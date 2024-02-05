#pragma once

namespace auth_service {

struct AccountDbInfo {
  std::string username;
  std::string email;
  std::string pass_salt;
  std::string pass_hash;
};

}  // namespace auth_service
