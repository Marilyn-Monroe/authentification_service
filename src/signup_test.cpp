#include "utils.hpp"

#include <userver/utest/utest.hpp>

UTEST(GetField, Base) {
  using auth_service::GetField;

  userver::formats::json::ValueBuilder request_data(
      userver::formats::json::Type::kObject);
  request_data["some_field_name"] = "some_value";
  EXPECT_EQ(
      GetField<std::string>(request_data.ExtractValue(), "some_field_name"),
      "some_value");
}

UTEST(GetField, FieldMissing) {
  using auth_service::GetField;

  userver::formats::json::ValueBuilder request_data(
      userver::formats::json::Type::kObject);
  EXPECT_THROW(
      GetField<std::string>(request_data.ExtractValue(), "some_field_name"),
      userver::server::handlers::ClientError);
}

UTEST(GetField, FieldTypeMismatch) {
  using auth_service::GetField;

  userver::formats::json::ValueBuilder request_data(
      userver::formats::json::Type::kObject);
  request_data["some_field_name"] = 100;
  EXPECT_THROW(
      GetField<std::string>(request_data.ExtractValue(), "some_field_name"),
      userver::server::handlers::ClientError);
}

UTEST(IsValidLength, Base) {
  using auth_service::IsValidLength;

  EXPECT_EQ(IsValidLength("valid_str", 9, 9), true);
  EXPECT_EQ(IsValidLength("valid_str", 5, 15), true);
  EXPECT_EQ(IsValidLength("valid_str"), true);
}

UTEST(IsValidLength, Min) {
  using auth_service::IsValidLength;

  EXPECT_EQ(IsValidLength("test", 10, 30), false);
  EXPECT_EQ(IsValidLength("test", 0, 30), true);
  EXPECT_EQ(IsValidLength("test", 4, 30), true);
}

UTEST(IsValidLength, Max) {
  using auth_service::IsValidLength;

  EXPECT_EQ(IsValidLength("test_string", 0, 20), true);
  EXPECT_EQ(IsValidLength("test_string", 0, 0), false);
  EXPECT_EQ(IsValidLength("test_string", 0, 11), true);
}