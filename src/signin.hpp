#pragma once

#include <userver/components/component_list.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/redis/client.hpp>

namespace auth_service {

namespace {

class SignIn final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName = "handler-signin";

  SignIn(const userver::components::ComponentConfig& config,
         const userver::components::ComponentContext& component_context);

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& json,
      userver::server::request::RequestContext&) const final;

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
  userver::storages::redis::ClientPtr redis_client_;
  userver::storages::redis::CommandControl redis_cc_;
};

}  // namespace

void AppendSignIn(userver::components::ComponentList& component_list);

}  // namespace auth_service
