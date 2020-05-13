/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <utility>

#include "base/strings/stringprintf.h"
#include "bat/ledger/internal/database/database_server_publisher_info.h"
#include "bat/ledger/internal/database/database_util.h"
#include "bat/ledger/internal/ledger_impl.h"

using std::placeholders::_1;

namespace {

const char kTableName[] = "server_publisher_info";

}  // namespace

namespace braveledger_database {

DatabaseServerPublisherInfo::DatabaseServerPublisherInfo(
    bat_ledger::LedgerImpl* ledger) :
    DatabaseTable(ledger),
    banner_(std::make_unique<DatabaseServerPublisherBanner>(ledger)) {
}

DatabaseServerPublisherInfo::~DatabaseServerPublisherInfo() = default;

bool DatabaseServerPublisherInfo::CreateTableV7(
    ledger::DBTransaction* transaction) {
  DCHECK(transaction);

  const std::string query = base::StringPrintf(
      "CREATE TABLE %s "
      "("
      "publisher_key LONGVARCHAR PRIMARY KEY NOT NULL UNIQUE,"
      "status INTEGER DEFAULT 0 NOT NULL,"
      "excluded INTEGER DEFAULT 0 NOT NULL,"
      "address TEXT NOT NULL"
      ")",
      kTableName);

  auto command = ledger::DBCommand::New();
  command->type = ledger::DBCommand::Type::EXECUTE;
  command->command = query;
  transaction->commands.push_back(std::move(command));

  return true;
}

bool DatabaseServerPublisherInfo::CreateIndexV7(
    ledger::DBTransaction* transaction) {
  DCHECK(transaction);

  return this->InsertIndex(transaction, kTableName, "publisher_key");
}

bool DatabaseServerPublisherInfo::CreateTableV21(
    ledger::DBTransaction* transaction) {
  DCHECK(transaction);

  const std::string query = base::StringPrintf(
      "CREATE TABLE %s "
      "("
      "publisher_key LONGVARCHAR PRIMARY KEY NOT NULL UNIQUE,"
      "status INTEGER DEFAULT 0 NOT NULL,"
      "excluded INTEGER DEFAULT 0 NOT NULL,"
      "address TEXT NOT NULL,"
      "updated_at INTEGER NOT NULL"
      ")",
      kTableName);

  auto command = ledger::DBCommand::New();
  command->type = ledger::DBCommand::Type::EXECUTE;
  command->command = query;
  transaction->commands.push_back(std::move(command));

  return true;
}

bool DatabaseServerPublisherInfo::CreateIndexV21(
    ledger::DBTransaction* transaction) {
  DCHECK(transaction);

  return this->InsertIndex(transaction, kTableName, "publisher_key");
}

bool DatabaseServerPublisherInfo::Migrate(
    ledger::DBTransaction* transaction,
    const int target) {
  DCHECK(transaction);

  switch (target) {
    case 7: {
      return MigrateToV7(transaction);
    }
    case 15: {
      return MigrateToV15(transaction);
    }
    case 21: {
      return MigrateToV21(transaction);
    }
    default: {
      return true;
    }
  }
}

bool DatabaseServerPublisherInfo::MigrateToV7(
    ledger::DBTransaction* transaction) {
  DCHECK(transaction);

  if (!DropTable(transaction, kTableName)) {
    return false;
  }

  if (!CreateTableV7(transaction)) {
    return false;
  }

  if (!CreateIndexV7(transaction)) {
    return false;
  }

  if (!banner_->Migrate(transaction, 7)) {
    return false;
  }

  return true;
}

bool DatabaseServerPublisherInfo::MigrateToV15(
    ledger::DBTransaction* transaction) {
  DCHECK(transaction);

  return banner_->Migrate(transaction, 15);
}

bool DatabaseServerPublisherInfo::MigrateToV21(
    ledger::DBTransaction* transaction) {
  DCHECK(transaction);

  // TODO(zenparsing): Do we need to vacuum after this? Vacuum
  // took 276ms and reduced the size from 137MB to 16MB.
  if (!DropTable(transaction, kTableName)) {
    return false;
  }

  if (!CreateTableV21(transaction)) {
    return false;
  }

  if (!CreateIndexV21(transaction)) {
    return false;
  }

  return banner_->Migrate(transaction, 21);
}

void DatabaseServerPublisherInfo::InsertOrUpdate(
    const ledger::ServerPublisherInfo& server_info,
    ledger::ResultCallback callback) {
  DCHECK(!server_info.publisher_key.empty());

  auto transaction = ledger::DBTransaction::New();

  auto command = ledger::DBCommand::New();
  command->type = ledger::DBCommand::Type::RUN;
  command->command = base::StringPrintf(
      "INSERT OR REPLACE INTO %s "
      "(publisher_key, status, excluded, address, updated_at) "
      "VALUES (?, ?, ?, ?, ?)",
      kTableName);

  LOG(INFO) << "[[zenparsing]] Inserting into table "
      << server_info.publisher_key << ", "
      << static_cast<int>(server_info.status) << ","
      << server_info.excluded << ","
      << server_info.address << ","
      << server_info.updated_at;

  BindString(command.get(), 0, server_info.publisher_key);
  BindInt(command.get(), 1, static_cast<int>(server_info.status));
  BindBool(command.get(), 2, server_info.excluded);
  BindString(command.get(), 3, server_info.address);
  BindInt64(command.get(), 4, server_info.updated_at);

  transaction->commands.push_back(std::move(command));
  banner_->InsertOrUpdate(transaction.get(), server_info);

  ledger_->RunDBTransaction(std::move(transaction),
      std::bind(&OnResultCallback, _1, callback));
}

void DatabaseServerPublisherInfo::GetRecord(
    const std::string& publisher_key,
    ledger::GetServerPublisherInfoCallback callback) {
  // Get banner first as is not complex struct where ServerPublisherInfo is
  auto banner_callback =
      std::bind(&DatabaseServerPublisherInfo::OnGetRecordBanner,
          this,
          _1,
          publisher_key,
          callback);

  banner_->GetRecord(publisher_key, banner_callback);
}

void DatabaseServerPublisherInfo::OnGetRecordBanner(
    ledger::PublisherBannerPtr banner,
    const std::string& publisher_key,
    ledger::GetServerPublisherInfoCallback callback) {
  auto transaction = ledger::DBTransaction::New();
  const std::string query = base::StringPrintf(
      "SELECT status, excluded, address, updated_at "
      "FROM %s WHERE publisher_key=?",
      kTableName);

  auto command = ledger::DBCommand::New();
  command->type = ledger::DBCommand::Type::READ;
  command->command = query;

  BindString(command.get(), 0, publisher_key);

  command->record_bindings = {
      ledger::DBCommand::RecordBindingType::INT_TYPE,
      ledger::DBCommand::RecordBindingType::BOOL_TYPE,
      ledger::DBCommand::RecordBindingType::STRING_TYPE,
      ledger::DBCommand::RecordBindingType::INT64_TYPE
  };

  transaction->commands.push_back(std::move(command));

  if (!banner) {
    banner = ledger::PublisherBanner::New();
  }

  auto transaction_callback =
      std::bind(&DatabaseServerPublisherInfo::OnGetRecord,
          this,
          _1,
          publisher_key,
          *banner,
          callback);

  ledger_->RunDBTransaction(std::move(transaction), transaction_callback);
}

void DatabaseServerPublisherInfo::OnGetRecord(
    ledger::DBCommandResponsePtr response,
    const std::string& publisher_key,
    const ledger::PublisherBanner& banner,
    ledger::GetServerPublisherInfoCallback callback) {
  if (!response ||
      response->status != ledger::DBCommandResponse::Status::RESPONSE_OK) {
    callback(nullptr);
    return;
  }

  if (response->result->get_records().size() != 1) {
    callback(nullptr);
    return;
  }

  auto* record = response->result->get_records()[0].get();

  auto info = ledger::ServerPublisherInfo::New();
  info->publisher_key = publisher_key;
  info->status = static_cast<ledger::mojom::PublisherStatus>(
      GetIntColumn(record, 0));
  info->excluded = GetBoolColumn(record, 1);
  info->address = GetStringColumn(record, 2);
  info->updated_at = GetInt64Column(record, 3);
  info->banner = banner.Clone();

  callback(std::move(info));
}

}  // namespace braveledger_database
