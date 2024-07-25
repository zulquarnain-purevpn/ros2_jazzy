// Copyright 2021 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <array>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

#include "gtest/gtest.h"

#include "rmw_dds_common/security.hpp"

// Utility to write test content on required files
template<size_t N>
static void write_test_content(const std::array<std::string, N> & required_files)
{
  for (const std::string & filename : required_files) {
    std::filesystem::path full_path = std::filesystem::path("./test_folder") / filename;
    std::ofstream output_buffer{full_path.generic_string()};
    output_buffer << "test";
    ASSERT_TRUE(std::filesystem::exists(full_path));
  }
}

// Utility to write pkcs11 content on required files
template<size_t N>
static void write_test_pkcs11_content(const std::array<std::string, N> & pkcs11_files)
{
  for (const std::string & filename : pkcs11_files) {
    std::filesystem::path full_path = std::filesystem::path("./test_folder") / filename;
    std::ofstream output_buffer{full_path.generic_string()};
    output_buffer << "pkcs11://" << filename;
    ASSERT_TRUE(std::filesystem::exists(full_path));
  }
}

class test_security : public ::testing::TestWithParam<bool> {};

TEST_P(test_security, files_exist_no_prefix)
{
  std::filesystem::path dir = std::filesystem::path("./test_folder");
  std::filesystem::remove_all(dir);
  EXPECT_TRUE(std::filesystem::create_directories(dir));
  EXPECT_TRUE(std::filesystem::exists(dir));
  EXPECT_TRUE(std::filesystem::is_directory(dir));

  std::array<std::string, 6> required_files = {
    "identity_ca.cert.pem", "cert.pem", "key.pem",
    "permissions_ca.cert.pem", "governance.p7s", "permissions.p7s"
  };
  write_test_content(required_files);

  std::unordered_map<std::string, std::string> security_files;
  ASSERT_TRUE(
    rmw_dds_common::get_security_files(GetParam(), "", dir.generic_string(), security_files));

  EXPECT_EQ(
    security_files["IDENTITY_CA"],
    std::filesystem::path("./test_folder/identity_ca.cert.pem").generic_string());
  EXPECT_EQ(
    security_files["CERTIFICATE"],
    std::filesystem::path("./test_folder/cert.pem").generic_string());
  EXPECT_EQ(
    security_files["PRIVATE_KEY"],
    std::filesystem::path("./test_folder/key.pem").generic_string());
  EXPECT_EQ(
    security_files["PERMISSIONS_CA"],
    std::filesystem::path("./test_folder/permissions_ca.cert.pem").generic_string());
  EXPECT_EQ(
    security_files["GOVERNANCE"],
    std::filesystem::path("./test_folder/governance.p7s").generic_string());
  EXPECT_EQ(
    security_files["PERMISSIONS"],
    std::filesystem::path("./test_folder/permissions.p7s").generic_string());
}

TEST_P(test_security, files_exist_with_prefix)
{
  std::filesystem::path dir = std::filesystem::path("./test_folder");
  std::filesystem::remove_all(dir);
  EXPECT_TRUE(std::filesystem::create_directories(dir));
  EXPECT_TRUE(std::filesystem::exists(dir));
  EXPECT_TRUE(std::filesystem::is_directory(dir));

  std::array<std::string, 6> required_files = {
    "identity_ca.cert.pem", "cert.pem", "key.pem",
    "permissions_ca.cert.pem", "governance.p7s", "permissions.p7s"
  };
  write_test_content(required_files);

  std::unordered_map<std::string, std::string> security_files;
  ASSERT_TRUE(
    rmw_dds_common::get_security_files(
      GetParam(), "file://", dir.generic_string(), security_files));

  EXPECT_EQ(
    security_files["IDENTITY_CA"],
    "file://" + std::filesystem::path("./test_folder/identity_ca.cert.pem").generic_string());
  EXPECT_EQ(
    security_files["CERTIFICATE"],
    "file://" + std::filesystem::path("./test_folder/cert.pem").generic_string());
  EXPECT_EQ(
    security_files["PRIVATE_KEY"],
    "file://" + std::filesystem::path("./test_folder/key.pem").generic_string());
  EXPECT_EQ(
    security_files["PERMISSIONS_CA"],
    "file://" + std::filesystem::path("./test_folder/permissions_ca.cert.pem").generic_string());
  EXPECT_EQ(
    security_files["GOVERNANCE"],
    "file://" + std::filesystem::path("./test_folder/governance.p7s").generic_string());
  EXPECT_EQ(
    security_files["PERMISSIONS"],
    "file://" + std::filesystem::path("./test_folder/permissions.p7s").generic_string());
}

TEST_P(test_security, file_missing)
{
  std::filesystem::path dir = std::filesystem::path("./test_folder");
  std::filesystem::remove_all(dir);
  EXPECT_TRUE(std::filesystem::create_directories(dir));
  EXPECT_TRUE(std::filesystem::exists(dir));
  EXPECT_TRUE(std::filesystem::is_directory(dir));

  std::array<std::string, 5> required_files = {
    "identity_ca.cert.pem", "cert.pem", "key.pem",
    "permissions_ca.cert.pem", "governance.p7s"
  };
  write_test_content(required_files);

  std::unordered_map<std::string, std::string> security_files;
  ASSERT_FALSE(
    rmw_dds_common::get_security_files(GetParam(), "", dir.generic_string(), security_files));
  ASSERT_EQ(security_files.size(), 0UL);
}

TEST_P(test_security, optional_file_exist)
{
  std::filesystem::path dir = std::filesystem::path("./test_folder");
  std::filesystem::remove_all(dir);
  EXPECT_TRUE(std::filesystem::create_directories(dir));
  EXPECT_TRUE(std::filesystem::exists(dir));
  EXPECT_TRUE(std::filesystem::is_directory(dir));

  std::array<std::string, 7> required_files = {
    "identity_ca.cert.pem", "cert.pem", "key.pem",
    "permissions_ca.cert.pem", "governance.p7s", "permissions.p7s", "crl.pem",
  };
  write_test_content(required_files);

  std::unordered_map<std::string, std::string> security_files;
  ASSERT_TRUE(
    rmw_dds_common::get_security_files(GetParam(), "", dir.generic_string(), security_files));

  EXPECT_EQ(
    security_files["IDENTITY_CA"],
    std::filesystem::path("./test_folder/identity_ca.cert.pem").generic_string());
  EXPECT_EQ(
    security_files["CERTIFICATE"],
    std::filesystem::path("./test_folder/cert.pem").generic_string());
  EXPECT_EQ(
    security_files["PRIVATE_KEY"],
    std::filesystem::path("./test_folder/key.pem").generic_string());
  EXPECT_EQ(
    security_files["PERMISSIONS_CA"],
    std::filesystem::path("./test_folder/permissions_ca.cert.pem").generic_string());
  EXPECT_EQ(
    security_files["GOVERNANCE"],
    std::filesystem::path("./test_folder/governance.p7s").generic_string());
  EXPECT_EQ(
    security_files["PERMISSIONS"],
    std::filesystem::path("./test_folder/permissions.p7s").generic_string());

  EXPECT_EQ(
    security_files["CRL"],
    std::filesystem::path("./test_folder/crl.pem").generic_string());
}

TEST_P(test_security, wrong_pkcs11_file_ignored)
{
  std::filesystem::path dir = std::filesystem::path("./test_folder");
  std::filesystem::remove_all(dir);
  EXPECT_TRUE(std::filesystem::create_directories(dir));
  EXPECT_TRUE(std::filesystem::exists(dir));
  EXPECT_TRUE(std::filesystem::is_directory(dir));

  std::array<std::string, 10> required_files = {
    "identity_ca.cert.pem", "cert.pem", "key.pem",
    "permissions_ca.cert.pem", "governance.p7s", "permissions.p7s",
    "identity_ca.cert.p11", "cert.p11", "key.p11",
    "permissions_ca.cert.p11"
  };
  write_test_content(required_files);

  std::unordered_map<std::string, std::string> security_files;
  ASSERT_TRUE(
    rmw_dds_common::get_security_files(GetParam(), "", dir.generic_string(), security_files));

  EXPECT_EQ(
    security_files["IDENTITY_CA"],
    std::filesystem::path("./test_folder/identity_ca.cert.pem").generic_string());
  EXPECT_EQ(
    security_files["CERTIFICATE"],
    std::filesystem::path("./test_folder/cert.pem").generic_string());
  EXPECT_EQ(
    security_files["PRIVATE_KEY"],
    std::filesystem::path("./test_folder/key.pem").generic_string());
  EXPECT_EQ(
    security_files["PERMISSIONS_CA"],
    std::filesystem::path("./test_folder/permissions_ca.cert.pem").generic_string());
  EXPECT_EQ(
    security_files["GOVERNANCE"],
    std::filesystem::path("./test_folder/governance.p7s").generic_string());
  EXPECT_EQ(
    security_files["PERMISSIONS"],
    std::filesystem::path("./test_folder/permissions.p7s").generic_string());
}

TEST_P(test_security, pkcs11_support_check)
{
  std::filesystem::path dir = std::filesystem::path("./test_folder");
  std::filesystem::remove_all(dir);
  EXPECT_TRUE(std::filesystem::create_directories(dir));
  EXPECT_TRUE(std::filesystem::exists(dir));
  EXPECT_TRUE(std::filesystem::is_directory(dir));

  std::array<std::string, 6> required_files = {
    "identity_ca.cert.pem", "cert.pem", "key.pem",
    "permissions_ca.cert.pem", "governance.p7s", "permissions.p7s"
  };
  write_test_content(required_files);

  std::array<std::string, 4> pkcs11_files = {
    "identity_ca.cert.p11", "cert.p11", "key.p11",
    "permissions_ca.cert.p11"
  };
  write_test_pkcs11_content(pkcs11_files);

  std::unordered_map<std::string, std::string> security_files;
  ASSERT_TRUE(
    rmw_dds_common::get_security_files(GetParam(), "", dir.generic_string(), security_files));

  if (GetParam()) {
    EXPECT_EQ(
      security_files["IDENTITY_CA"],
      "pkcs11://identity_ca.cert.p11");
    EXPECT_EQ(
      security_files["CERTIFICATE"],
      "pkcs11://cert.p11");
    EXPECT_EQ(
      security_files["PRIVATE_KEY"],
      "pkcs11://key.p11");
    EXPECT_EQ(
      security_files["PERMISSIONS_CA"],
      "pkcs11://permissions_ca.cert.p11");
  } else {
    EXPECT_EQ(
      security_files["IDENTITY_CA"],
      std::filesystem::path("./test_folder/identity_ca.cert.pem").generic_string());
    EXPECT_EQ(
      security_files["CERTIFICATE"],
      std::filesystem::path("./test_folder/cert.pem").generic_string());
    EXPECT_EQ(
      security_files["PRIVATE_KEY"],
      std::filesystem::path("./test_folder/key.pem").generic_string());
    EXPECT_EQ(
      security_files["PERMISSIONS_CA"],
      std::filesystem::path("./test_folder/permissions_ca.cert.pem").generic_string());
  }
  EXPECT_EQ(
    security_files["GOVERNANCE"],
    std::filesystem::path("./test_folder/governance.p7s").generic_string());
  EXPECT_EQ(
    security_files["PERMISSIONS"],
    std::filesystem::path("./test_folder/permissions.p7s").generic_string());
}

TEST_P(test_security, only_pkcs11_present)
{
  std::filesystem::path dir = std::filesystem::path("./test_folder");
  std::filesystem::remove_all(dir);
  EXPECT_TRUE(std::filesystem::create_directories(dir));
  EXPECT_TRUE(std::filesystem::exists(dir));
  EXPECT_TRUE(std::filesystem::is_directory(dir));

  std::array<std::string, 2> required_files = {
    "governance.p7s", "permissions.p7s"
  };
  write_test_content(required_files);

  std::array<std::string, 4> pkcs11_files = {
    "identity_ca.cert.p11", "cert.p11", "key.p11",
    "permissions_ca.cert.p11"
  };
  write_test_pkcs11_content(pkcs11_files);

  std::unordered_map<std::string, std::string> security_files;

  if (GetParam()) {
    ASSERT_TRUE(
      rmw_dds_common::get_security_files(GetParam(), "", dir.generic_string(), security_files));
    EXPECT_EQ(
      security_files["IDENTITY_CA"],
      "pkcs11://identity_ca.cert.p11");
    EXPECT_EQ(
      security_files["CERTIFICATE"],
      "pkcs11://cert.p11");
    EXPECT_EQ(
      security_files["PRIVATE_KEY"],
      "pkcs11://key.p11");
    EXPECT_EQ(
      security_files["PERMISSIONS_CA"],
      "pkcs11://permissions_ca.cert.p11");
    EXPECT_EQ(
      security_files["GOVERNANCE"],
      std::filesystem::path("./test_folder/governance.p7s").generic_string());
    EXPECT_EQ(
      security_files["PERMISSIONS"],
      std::filesystem::path("./test_folder/permissions.p7s").generic_string());
  } else {
    ASSERT_FALSE(
      rmw_dds_common::get_security_files(GetParam(), "", dir.generic_string(), security_files));
    ASSERT_EQ(security_files.size(), 0UL);
  }
}

TEST_P(test_security, pkcs11_prefix_ignored)
{
  std::filesystem::path dir = std::filesystem::path("./test_folder");
  std::filesystem::remove_all(dir);
  EXPECT_TRUE(std::filesystem::create_directories(dir));
  EXPECT_TRUE(std::filesystem::exists(dir));
  EXPECT_TRUE(std::filesystem::is_directory(dir));

  std::array<std::string, 6> required_files = {
    "identity_ca.cert.pem", "cert.pem", "key.pem",
    "permissions_ca.cert.pem", "governance.p7s", "permissions.p7s"
  };
  write_test_content(required_files);

  std::array<std::string, 4> pkcs11_files = {
    "identity_ca.cert.p11", "cert.p11", "key.p11",
    "permissions_ca.cert.p11"
  };
  write_test_pkcs11_content(pkcs11_files);

  std::unordered_map<std::string, std::string> security_files;
  ASSERT_TRUE(
    rmw_dds_common::get_security_files(
      GetParam(), "file://", dir.generic_string(), security_files));

  if (GetParam()) {
    EXPECT_EQ(
      security_files["IDENTITY_CA"],
      "pkcs11://identity_ca.cert.p11");
    EXPECT_EQ(
      security_files["CERTIFICATE"],
      "pkcs11://cert.p11");
    EXPECT_EQ(
      security_files["PRIVATE_KEY"],
      "pkcs11://key.p11");
    EXPECT_EQ(
      security_files["PERMISSIONS_CA"],
      "pkcs11://permissions_ca.cert.p11");
  } else {
    EXPECT_EQ(
      security_files["IDENTITY_CA"],
      "file://" + std::filesystem::path("./test_folder/identity_ca.cert.pem").generic_string());
    EXPECT_EQ(
      security_files["CERTIFICATE"],
      "file://" + std::filesystem::path("./test_folder/cert.pem").generic_string());
    EXPECT_EQ(
      security_files["PRIVATE_KEY"],
      "file://" + std::filesystem::path("./test_folder/key.pem").generic_string());
    EXPECT_EQ(
      security_files["PERMISSIONS_CA"],
      "file://" + std::filesystem::path("./test_folder/permissions_ca.cert.pem").generic_string());
  }
  EXPECT_EQ(
    security_files["GOVERNANCE"],
    "file://" + std::filesystem::path("./test_folder/governance.p7s").generic_string());
  EXPECT_EQ(
    security_files["PERMISSIONS"],
    "file://" + std::filesystem::path("./test_folder/permissions.p7s").generic_string());
}

INSTANTIATE_TEST_SUITE_P(
  test_security,
  test_security,
  ::testing::Values(false, true),
  [](const testing::TestParamInfo<bool> & info) {
    return info.param ? "with_pkcs11_support" : "with_no_pkcs11_support";
  });
