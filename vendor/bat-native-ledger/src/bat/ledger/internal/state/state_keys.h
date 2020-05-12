/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_STATE_STATE_KEYS_H_
#define BRAVELEDGER_STATE_STATE_KEYS_H_

#include <string>

namespace ledger {
  const char kStateEnabled[] = "enabled";
  const char kStateEnabledMigrated[] = "enabled_migrated";
  const char kStateServerPublisherListStamp[] = "server_publisher_list_stamp";
  const char kStateUpholdAnonAddress[] = "uphold_anon_address";
  const char kStatePromotionLastFetchStamp[] = "promotion_last_fetch_stamp";
  const char kStatePromotionCorruptedMigrated[] =
      "promotion_corrupted_migrated2";
  const char kStateAnonTransferChecked[] = "anon_transfer_checked";
  const char kStateVersion[] = "version";
  const char kStateMinVisitTime[] = "ac.min_visit_time";
  const char kStateMinVisits[] = "ac.min_visits";
  const char kStateAllowNonVerified[] = "ac.allow_non_verified";
  const char kStateAllowVideoContribution[] = "ac.allow_video_contributions";
  const char kStateScoreA[] = "ac.score.a";
  const char kStateScoreB[] = "ac.score.b";

}  // namespace ledger

#endif  // BRAVELEDGER_STATE_STATE_KEYS_H_
