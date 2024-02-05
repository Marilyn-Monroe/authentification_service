#include "signin.hpp"
#include "account_info.hpp"
#include "utils.hpp"

#include <fmt/format.h>

#include <userver/components/component.hpp>
#include <userver/logging/log.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/uuid4.hpp>

namespace auth_service {

namespace {

SignIn::SignIn(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      pg_cluster_(
          component_context
              .FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()) {}

userver::formats::json::Value SignIn::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& json,
    userver::server::request::RequestContext&) const {
  if (!json.HasMember("login")) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"No 'login' argument"});
  }

  if (!json["login"].IsString()) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            "Invalid 'login' argument type"});
  }

  const auto& login = json["login"].As<std::string>();
  if (login.length() < 6 || login.length() > 256) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"Wrong 'login' argument"});
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

  auto selectResult = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT username, email, pass_salt, pass_hash FROM accounts "
      "WHERE username = $1 OR email = $2",
      login, login);

  if (selectResult.IsEmpty()) {
    throw userver::server::handlers::Unauthorized(
        userver::server::handlers::ExternalBody{"Bad credentials"});
  }

  auto account_db_info = selectResult.AsSingleRow<AccountDbInfo>(
      userver::storages::postgres::kRowTag);

  const auto hash = PBKDF2_HMAC_SHA_512(password, account_db_info.pass_salt);

  if (hash != account_db_info.pass_hash) {
    throw userver::server::handlers::Unauthorized(
        userver::server::handlers::ExternalBody{"Bad credentials"});
  }

  userver::formats::json::ValueBuilder response(
      userver::formats::common::Type::kObject);
  response["sessionid"] = userver::utils::generators::GenerateUuid();

  return response.ExtractValue();
}

}  // namespace

void AppendSignIn(userver::components::ComponentList& component_list) {
  component_list.Append<SignIn>();
}

}  // namespace auth_service
