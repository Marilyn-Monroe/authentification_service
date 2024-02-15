#include "utils.hpp"

#include <userver/utest/utest.hpp>

UTEST(GetField, Base) {
  using authentification_service::GetField;

  userver::formats::json::ValueBuilder request_data(
      userver::formats::json::Type::kObject);
  request_data["some_field_name"] = "some_value";
  EXPECT_EQ(
      GetField<std::string>(request_data.ExtractValue(), "some_field_name"),
      "some_value");
}

UTEST(GetField, FieldMissing) {
  using authentification_service::GetField;

  userver::formats::json::ValueBuilder request_data(
      userver::formats::json::Type::kObject);
  EXPECT_THROW(
      GetField<std::string>(request_data.ExtractValue(), "some_field_name"),
      userver::server::handlers::ClientError);
}

UTEST(GetField, FieldTypeMismatch) {
  using authentification_service::GetField;

  userver::formats::json::ValueBuilder request_data(
      userver::formats::json::Type::kObject);
  request_data["some_field_name"] = 100;
  EXPECT_THROW(
      GetField<std::string>(request_data.ExtractValue(), "some_field_name"),
      userver::server::handlers::ClientError);
}

UTEST(IsValidLength, Base) {
  using authentification_service::IsValidLength;

  EXPECT_EQ(IsValidLength("valid_str", 9, 9), true);
  EXPECT_EQ(IsValidLength("valid_str", 5, 15), true);
  EXPECT_EQ(IsValidLength("valid_str"), true);
}

UTEST(IsValidLength, Min) {
  using authentification_service::IsValidLength;

  EXPECT_EQ(IsValidLength("test", 10, 30), false);
  EXPECT_EQ(IsValidLength("test", 0, 30), true);
  EXPECT_EQ(IsValidLength("test", 4, 30), true);
}

UTEST(IsValidLength, Max) {
  using authentification_service::IsValidLength;

  EXPECT_EQ(IsValidLength("test_string", 0, 20), true);
  EXPECT_EQ(IsValidLength("test_string", 0, 0), false);
  EXPECT_EQ(IsValidLength("test_string", 0, 11), true);
}

UTEST(ValidateUsername, Length) {
  using authentification_service::ValidateUsername;

  EXPECT_NO_THROW(ValidateUsername("test_username"));
  EXPECT_THROW(ValidateUsername("user"),
               userver::server::handlers::ClientError);
  EXPECT_THROW(ValidateUsername("authentification_service_length_username_value"),
               userver::server::handlers::ClientError);
}

UTEST(ValidateEmail, Length) {
  using authentification_service::ValidateEmail;

  EXPECT_NO_THROW(ValidateEmail("correct_email@test.com"));
  EXPECT_THROW(ValidateEmail("wrong"), userver::server::handlers::ClientError);
  EXPECT_THROW(
      ValidateEmail(
          "contact-admin-hello-webmaster-info-services-peter-crazy-but-oh-so-"
          "ubber-cool-english-alphabet-loverer-abcdefghijklmnopqrstuvwxyz@"
          "please-try-to.send-me-an-email-if-you-can-possibly-begin-to-"
          "remember-this-coz.this-is-the-longest-email-address-known-to-man-"
          "but-to-be-honest.this-is-such-a-stupidly-long-sub-domain-it-could-"
          "go-on-forever.pacraig.com"),
      userver::server::handlers::ClientError);
}

UTEST(ValidatePassword, Length) {
  using authentification_service::ValidatePassword;

  EXPECT_NO_THROW(ValidatePassword("valid_password"));
  EXPECT_THROW(ValidatePassword("wrong"),
               userver::server::handlers::ClientError);
  EXPECT_THROW(
      ValidatePassword(
          "zTo5Vk5PzHVM2Zq2RyAKF6djdzpJQbFOHfKo0P2cCBvl58TbO9zTo5Vk5PzHVM2Zq2Ry"
          "AKF6djdzpJQbFOHfKo0P2cCBvl58TbO9zTo5Vk5PzHVM2Zq2RyAKF6djdzpJQbFOHfKo"
          "0P2cCBvl58TbO9zTo5Vk5PzHVM2Zq2RyAKF6djdzpJQbFOHfKo0P2cCBvl58TbO9zTo5"
          "Vk5PzHVM2Zq2RyAKF6djdzpJQbFOHfKo0P2cCBvl58TbO9P2cCBvl58TbO9"),
      userver::server::handlers::ClientError);
}

UTEST(ValidateLogin, Length) {
  using authentification_service::ValidateLogin;

  EXPECT_NO_THROW(ValidateLogin("correct_email@test.com"));
  EXPECT_NO_THROW(ValidateLogin("correct_username"));
  EXPECT_THROW(ValidateLogin("wrong"), userver::server::handlers::ClientError);
  EXPECT_THROW(
      ValidateLogin(
          "contact-admin-hello-webmaster-info-services-peter-crazy-but-oh-so-"
          "ubber-cool-english-alphabet-loverer-abcdefghijklmnopqrstuvwxyz@"
          "please-try-to.send-me-an-email-if-you-can-possibly-begin-to-"
          "remember-this-coz.this-is-the-longest-email-address-known-to-man-"
          "but-to-be-honest.this-is-such-a-stupidly-long-sub-domain-it-could-"
          "go-on-forever.pacraig.com"),
      userver::server::handlers::ClientError);
}