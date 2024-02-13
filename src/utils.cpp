#include "utils.hpp"

#include <openssl/evp.h>

namespace auth_service {

std::string PBKDF2_HMAC_SHA_512(const std::string& password,
                                const std::string& salt) {
  const int kIterations = 16384;
  const int kOutputBytes = 64;
  unsigned char digest[kOutputBytes];
  PKCS5_PBKDF2_HMAC(password.c_str(), password.length(),
                    reinterpret_cast<const unsigned char*>(salt.c_str()),
                    salt.length(), kIterations, EVP_sha512(), kOutputBytes,
                    digest);

  std::string result;
  for (unsigned char i : digest) {
    result.append(fmt::format("{:02x}", 255 & i));
  }

  return result;
}

bool IsValidLength(const std::string& value, size_t min_len, size_t max_len) {
  return value.length() >= min_len && value.length() <= max_len;
}

void ValidateUsername(const std::string& username) {
  if (!IsValidLength(username, 6, 32)) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"Wrong 'username' argument"});
  }
}

void ValidateEmail(const std::string& email) {
  if (!IsValidLength(email, 6, 256)) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"Wrong 'email' argument"});
  }
}

void ValidatePassword(const std::string& password) {
  if (!IsValidLength(password, 8, 256)) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"Wrong 'password' argument"});
  }
}

void ValidateLogin(const std::string& login) {
  if (!IsValidLength(login, 6, 256)) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"Wrong 'login' argument"});
  }
}

}  // namespace auth_service
