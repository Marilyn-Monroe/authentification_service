#include "sessions_management_client.hpp"

#include "userver/yaml_config/merge_schemas.hpp"

namespace authentification_service {

SessionsManagementClient::SessionsManagementClient(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::components::LoggableComponentBase(config, component_context),
      client_factory_(
          component_context
              .FindComponent<userver::ugrpc::client::ClientFactoryComponent>()
              .GetFactory()),
      // The client needs a fixed endpoint
      client_(client_factory_
                  .MakeClient<handlers::api::SessionsManagementServiceClient>(
                      "sessions-management-client",
                      config["endpoint"].As<std::string>())) {}

std::string SessionsManagementClient::CreateSession(std::string user_id) {
  handlers::api::CreateSessionRequest request;
  request.set_user_id(std::move(user_id));

  // Deadline must be set manually for each RPC
  auto context = std::make_unique<grpc::ClientContext>();
  context->set_deadline(
      userver::engine::Deadline::FromDuration(std::chrono::seconds{20}));

  // Initiate the RPC. No actual actions have been taken thus far besides
  // preparing to send the request.
  auto stream = client_.CreateSession(request, std::move(context));

  // Complete the unary RPC by sending the request and receiving the response.
  // The client should call `Finish` (in case of single response) or `Read`
  // until `false` (in case of response stream), otherwise the RPC will be
  // cancelled.
  handlers::api::CreateSessionResponse response = stream.Finish();

  return std::move(*response.mutable_session_id());
}

userver::yaml_config::Schema SessionsManagementClient::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<
      userver::components::LoggableComponentBase>(R"(
type: object
description: >
    a user-defined wrapper around api::GreeterServiceClient that provides
    a simplified interface.
additionalProperties: false
properties:
    endpoint:
        type: string
        description: >
            Some other service endpoint (URI).
)");
}

void AppendSessionsManagementClient(
    userver::components::ComponentList& component_list) {
  component_list.Append<SessionsManagementClient>();
}

}  // namespace authentification_service
