#pragma once

#include <userver/utils/boost_uuid4.hpp>

namespace authentification_service {

struct AccountDbInfo {
  boost::uuids::uuid id;
  std::string username;
  std::string email;
  std::string pass_salt;
  std::string pass_hash;
};

}  // namespace authentification_service
