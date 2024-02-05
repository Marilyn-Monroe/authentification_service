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
  if (!json.HasMember("username")) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"No 'username' argument"});
  }

  if (!json["username"].IsString()) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            "Invalid 'username' argument type"});
  }

  const auto& username = json["username"].As<std::string>();
  if (username.length() < 6 || username.length() > 32) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"Wrong 'username' argument"});
  }

  if (!json.HasMember("email")) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"No 'email' argument"});
  }

  if (!json["email"].IsString()) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            "Invalid 'email' argument type"});
  }

  const auto& email = json["email"].As<std::string>();
  if (email.length() < 6 || email.length() > 256) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"Wrong 'email' argument"});
  }

  if (!json.HasMember("password")) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"No 'password' argument"});
  }

  if (!json["password"].IsString()) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            "Invalid 'password' argument type"});
  }

  const auto& password = json["password"].As<std::string>();
  if (password.length() < 8 || password.length() > 256) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"Wrong 'password' argument"});
  }

  auto selectResult =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           "SELECT 1 FROM accounts "
                           "WHERE username = $1 OR email = $2",
                           username, email);

  if (!selectResult.IsEmpty()) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"Account already exists"});
  }

  const auto salt = userver::utils::generators::GenerateUuid();
  const auto hash = PBKDF2_HMAC_SHA_512(password, salt);

  auto insertResult = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO accounts(username, email, pass_salt, pass_hash) "
      "VALUES($1, $2, $3, $4) "
      "RETURNING accounts.id",
      username, email, salt, hash);

  userver::formats::json::ValueBuilder response(
      userver::formats::common::Type::kObject);
  response["id"] = insertResult.AsSingleRow<int>();

  request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
  return response.ExtractValue();
}

}  // namespace

void AppendSignUp(userver::components::ComponentList& component_list) {
  component_list.Append<SignUp>();
}

}  // namespace auth_service
