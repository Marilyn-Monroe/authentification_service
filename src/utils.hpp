#pragma once

#include <string>

namespace auth_service {

std::string PBKDF2_HMAC_SHA_512(const std::string password,
                                const std::string salt);

}  // namespace auth_service
