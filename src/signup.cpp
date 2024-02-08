#include "signup.hpp"
#include "utils.hpp"

#include <fmt/format.h>

#include <userver/components/component.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/uuid4.hpp>

namespace auth_service {

namespace {

SignUp::SignUp(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      pg_cluster_(
          component_context
              .FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()) {}

userver::formats::json::Value SignUp::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& json,
    userver::server::request::RequestContext&) const {
  const auto& username = GetField<std::string>(json, "username");
  ValidateUsername(username);

  const auto& email = GetField<std::string>(json, "email");
  ValidateEmail(email);

  const auto& password = GetField<std::string>(json, "password");
  ValidatePassword(password);

  auto select_result =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           "SELECT 1 FROM accounts "
                           "WHERE username = $1 OR email = $2",
                           username, email);

  if (!select_result.IsEmpty()) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"Account already exists"});
  }

  const auto& salt = userver::utils::generators::GenerateUuid();
  const auto& hash = PBKDF2_HMAC_SHA_512(password, salt);

  auto insert_result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO accounts(username, email, pass_salt, pass_hash) "
      "VALUES($1, $2, $3, $4) "
      "RETURNING accounts.id",
      username, email, salt, hash);

  userver::formats::json::ValueBuilder response(
      userver::formats::common::Type::kObject);
  response["id"] = insert_result.AsSingleRow<int>();

  request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
  return response.ExtractValue();
}

}  // namespace

void AppendSignUp(userver::components::ComponentList& component_list) {
  component_list.Append<SignUp>();
}

}  // namespace auth_service
