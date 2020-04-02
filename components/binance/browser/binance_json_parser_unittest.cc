/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/binance/browser/binance_json_parser.h"

#include "brave/components/content_settings/core/common/content_settings_util.h"
#include "chrome/test/base/chrome_render_view_host_test_harness.h"

namespace {

std::string GetBalanceFromAssets(
    const std::map<std::string, std::string>& balances,
    const std::string& asset) {
  std::string balance;
  std::map<std::string, std::string>::const_iterator it =
      balances.find(asset);
  if (it != balances.end()) {
    balance = it->second;
  }
  return balance;
}

typedef testing::Test BinanceJSONParserTest;

TEST_F(BinanceJSONParserTest, GetAccountBalancesFromJSON) {
  std::map<std::string, std::string> balances;
  ASSERT_TRUE(BinanceJSONParser::GetAccountBalancesFromJSON(R"(
      {
        "code": "000000",
        "message": null,
        "data": [
          {
            "asset": "BNB",
            "free": "10114.00000000",
            "locked": "0.00000000",
            "freeze": "999990.00000000",
            "withdrawing": "0.00000000"
          },
          {
            "asset": "BTC",
            "free": "2.45000000",
            "locked": "0.00000000",
            "freeze": "999990.00000000",
            "withdrawing": "0.00000000"
          }
        ]
      })", &balances));

  std::string bnb_balance = GetBalanceFromAssets(balances, "BNB");
  std::string btc_balance = GetBalanceFromAssets(balances, "BTC");
  ASSERT_EQ(bnb_balance, "10114.00000000");
  ASSERT_EQ(btc_balance, "2.45000000");
}

TEST_F(BinanceJSONParserTest, GetTokensFromJSON) {
  std::string access_token;
  std::string refresh_token;

  // Tokens are taken from documentation, examples only
  ASSERT_TRUE(BinanceJSONParser::GetTokensFromJSON(R"(
      {
        "access_token": "83f2bf51-a2c4-4c2e-b7c4-46cef6a8dba5",
        "refresh_token": "fb5587ee-d9cf-4cb5-a586-4aed72cc9bea",
        "scope": "read",
        "token_type": "bearer",
        "expires_in": 30714
      })", &access_token, "access_token"));

  ASSERT_TRUE(BinanceJSONParser::GetTokensFromJSON(R"(
      {
        "access_token": "83f2bf51-a2c4-4c2e-b7c4-46cef6a8dba5",
        "refresh_token": "fb5587ee-d9cf-4cb5-a586-4aed72cc9bea",
        "scope": "read",
        "token_type": "bearer",
        "expires_in": 30714
      })", &refresh_token, "refresh_token"));

  ASSERT_EQ(access_token, "83f2bf51-a2c4-4c2e-b7c4-46cef6a8dba5");
  ASSERT_EQ(refresh_token, "fb5587ee-d9cf-4cb5-a586-4aed72cc9bea");
}

TEST_F(BinanceJSONParserTest, GetQuoteIDFromJSON) {
  std::string quote_id;
  ASSERT_TRUE(BinanceJSONParser::GetQuoteIDFromJSON(R"(
      {
        "code": "12345",
        "data": {
          "quoteId" : "12345"
        }
      })", &quote_id));

  ASSERT_EQ(quote_id, "12345");
}

TEST_F(BinanceJSONParserTest, GetTickerPriceFromJSON) {
  std::string symbol_pair_price;
  ASSERT_TRUE(BinanceJSONParser::GetTickerPriceFromJSON(R"(
      {
        "symbol": "BTCUSDT",
        "price": "7137.98000000"
      })", &symbol_pair_price));
  ASSERT_EQ(symbol_pair_price, "7137.98000000");
}

TEST_F(BinanceJSONParserTest, GetTickerVolumeFromJSON) {
  std::string symbol_pair_volume;
  ASSERT_TRUE(BinanceJSONParser::GetTickerVolumeFromJSON(R"(
      {
        "symbol": "BTCUSDT",
        "volume": "99849.90399800"
      })", &symbol_pair_volume));
  ASSERT_EQ(symbol_pair_volume, "99849.90399800");
}

TEST_F(BinanceJSONParserTest, GetDepositInfoFromJSON) {
  std::string deposit_address;
  std::string deposit_url;
  ASSERT_TRUE(BinanceJSONParser::GetDepositInfoFromJSON(R"(
      {
        "code": "0000",
        "message": "null",
        "data": {
          "coin": "BTC",
          "address": "112tfsHDk6Yk8PbNnTVkv7yPox4aWYYDtW",
          "url": "https://btc.com/112tfsHDk6Yk8PbNnTVkv7yPox4aWYYDtW",
          "time": 1566366289000
        }
      })", &deposit_address, &deposit_url));
  ASSERT_EQ(deposit_address, "112tfsHDk6Yk8PbNnTVkv7yPox4aWYYDtW");
  ASSERT_EQ(deposit_url, "https://btc.com/112tfsHDk6Yk8PbNnTVkv7yPox4aWYYDtW");
}

}  // namespace
