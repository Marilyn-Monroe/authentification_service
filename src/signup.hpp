#pragma once

#include <userver/components/component_list.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace auth_service {

namespace {

class SignUp final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "handler-signup";

  SignUp(const userver::components::ComponentConfig& config,
         const userver::components::ComponentContext& component_context);

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& json,
      userver::server::request::RequestContext&) const override final;

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

std::string PBKDF2_HMAC_SHA_512(const std::string password,
                                const std::string salt);

void AppendSignUp(userver::components::ComponentList& component_list);

}  // namespace auth_service