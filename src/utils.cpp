#include "utils.hpp"

#include <fmt/format.h>

#include <openssl/evp.h>

namespace auth_service {

std::string PBKDF2_HMAC_SHA_512(const std::string password,
                                const std::string salt) {
  const int kIterations = 16384;
  const int kOutputBytes = 64;
  unsigned char digest[kOutputBytes];
  PKCS5_PBKDF2_HMAC(password.c_str(), password.length(),
                    reinterpret_cast<const unsigned char*>(salt.c_str()),
                    salt.length(), kIterations, EVP_sha512(), kOutputBytes,
                    digest);

  std::string result;
  for (size_t i = 0; i < sizeof(digest); i++) {
    result.append(fmt::format("{:02x}", 255 & digest[i]));
  }

  return result;
}

}  // namespace auth_service
