#pragma once

#include <cstdint>

namespace auth_service {

struct AccountDbInfo {
  int32_t id;
  std::string username;
  std::string email;
  std::string pass_salt;
  std::string pass_hash;
};

}  // namespace auth_service
