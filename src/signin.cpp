#include "signin.hpp"
#include "account_info.hpp"
#include "utils.hpp"

#include <chrono>

#include <userver/components/component.hpp>
#include <userver/storages/postgres/component.hpp>

namespace authentification_service {

namespace {

SignIn::SignIn(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      pg_cluster_(
          component_context
              .FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()),
      client_(component_context.FindComponent<SessionsManagementClient>()) {}

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

  std::string session_id;
  try {
    session_id =
        client_.CreateSession(userver::utils::ToString(account_db_info.id));
  } catch (const userver::ugrpc::client::UnavailableError& e) {
    throw userver::server::handlers::InternalServerError(
        userver::server::handlers::ExternalBody{"Some internal error"});
  }

  userver::server::http::Cookie cookie{"sessionid", session_id};
  cookie.SetSecure().SetHttpOnly();
  const auto thirty_days = std::chrono::hours{24 * 30};
  cookie.SetMaxAge(thirty_days);
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
