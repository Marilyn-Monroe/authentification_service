#pragma once

#include <cstdint>

namespace authentification_service {

struct AccountDbInfo {
  int32_t id;
  std::string username;
  std::string email;
  std::string pass_salt;
  std::string pass_hash;
};

}  // namespace authentification_service
