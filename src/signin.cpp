#include "signin.hpp"
#include "account_info.hpp"
#include "utils.hpp"

#include <chrono>

#include <userver/components/component.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/redis/component.hpp>
#include <userver/utils/uuid4.hpp>

namespace authentification_service {

namespace {

SignIn::SignIn(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      pg_cluster_(
          component_context
              .FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()),
      redis_client_{component_context
                        .FindComponent<userver::components::Redis>("redis-db-1")
                        .GetClient("redis_db_1")},
      redis_cc_{std::chrono::seconds{15}, std::chrono::seconds{60}, 4} {}

userver::formats::json::Value SignIn::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& json,
    userver::server::request::RequestContext&) const {
  const auto& login = GetField<std::string>(json, "login");
  ValidateLogin(login);

  const auto& password = GetField<std::string>(json, "password");
  ValidatePassword(password);

  auto select_result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT id, username, email, pass_salt, pass_hash FROM accounts "
      "WHERE username = $1 OR email = $2",
      login, login);

  if (select_result.IsEmpty()) {
    throw userver::server::handlers::Unauthorized(
        userver::server::handlers::ExternalBody{"Bad credentials"});
  }

  auto account_db_info = select_result.AsSingleRow<AccountDbInfo>(
      userver::storages::postgres::kRowTag);

  const auto hash = PBKDF2_HMAC_SHA_512(password, account_db_info.pass_salt);

  if (hash != account_db_info.pass_hash) {
    throw userver::server::handlers::Unauthorized(
        userver::server::handlers::ExternalBody{"Bad credentials"});
  }

  const auto thirtyDays = std::chrono::hours{24 * 30};
  auto session_id = userver::utils::generators::GenerateUuid();
  session_id.append(std::to_string(account_db_info.id));
  try {
    redis_client_
        ->Set(session_id, std::to_string(account_db_info.id),
              thirtyDays, redis_cc_)
        .Get();
  } catch (const userver::redis::RequestFailedException& e) {
    throw userver::server::handlers::InternalServerError(
        userver::server::handlers::ExternalBody{
            "Some kind of internal error. Try again later."});
  }

  userver::server::http::Cookie cookie{"sessionid", session_id};
  cookie.SetSecure().SetHttpOnly();
  cookie.SetMaxAge(thirtyDays);
  cookie.SetPath("/");
  cookie.SetSameSite("Strict");

  request.GetHttpResponse().SetCookie(cookie);

  userver::formats::json::ValueBuilder response(
      userver::formats::common::Type::kObject);
  response["sessionid"] = session_id;

  return response.ExtractValue();
}

}  // namespace

void AppendSignIn(userver::components::ComponentList& component_list) {
  component_list.Append<SignIn>();
}

}  // namespace authentification_service
