#pragma once

#include <limits>
#include <string>

#include <fmt/format.h>

#include <userver/formats/json.hpp>
#include <userver/server/handlers/exceptions.hpp>

namespace auth_service {

std::string PBKDF2_HMAC_SHA_512(const std::string& password,
                                const std::string& salt);

template <typename Type>
Type GetField(const userver::formats::json::Value& json,
              const std::string& field_name) {
  Type field;
  try {
    field = json[field_name].As<Type>();
  } catch (const userver::formats::json::MemberMissingException&) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            fmt::format("No '{}' argument", field_name)});
  } catch (const userver::formats::json::TypeMismatchException&) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            fmt::format("Invalid '{}' argument type", field_name)});
  }
  return field;
}

bool IsValidLength(const std::string& value,
                   size_t min_len = std::numeric_limits<size_t>::min(),
                   size_t max_len = std::numeric_limits<size_t>::max());

void ValidateUsername(const std::string& username);
void ValidateEmail(const std::string& email);
void ValidatePassword(const std::string& password);

}  // namespace auth_service
