// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include <algorithm>
#include <cstddef>
#include <utility>

#include "arrow/util/key_value_metadata.h"
#include "arrow/util/logging.h"

using std::size_t;

namespace arrow {

static std::vector<std::string> UnorderedMapKeys(
    const std::unordered_map<std::string, std::string>& map) {
  std::vector<std::string> keys;
  keys.reserve(map.size());
  for (const auto& pair : map) {
    keys.push_back(pair.first);
  }
  return keys;
}

static std::vector<std::string> UnorderedMapValues(
    const std::unordered_map<std::string, std::string>& map) {
  std::vector<std::string> values;
  values.reserve(map.size());
  for (const auto& pair : map) {
    values.push_back(pair.second);
  }
  return values;
}

KeyValueMetadata::KeyValueMetadata() : keys_(), values_() {}

KeyValueMetadata::KeyValueMetadata(
    const std::unordered_map<std::string, std::string>& map)
    : keys_(UnorderedMapKeys(map)), values_(UnorderedMapValues(map)) {
  DCHECK_EQ(keys_.size(), values_.size());
}

KeyValueMetadata::KeyValueMetadata(const std::vector<std::string>& keys,
                                   const std::vector<std::string>& values)
    : keys_(keys), values_(values) {
  DCHECK_EQ(keys.size(), values.size());
}

void KeyValueMetadata::ToUnorderedMap(
    std::unordered_map<std::string, std::string>* out) const {
  DCHECK_NE(out, nullptr);
  const int64_t n = size();
  out->reserve(n);
  for (int64_t i = 0; i < n; ++i) {
    out->insert(std::make_pair(key(i), value(i)));
  }
}

void KeyValueMetadata::Append(const std::string& key, const std::string& value) {
  keys_.push_back(key);
  values_.push_back(value);
}

void KeyValueMetadata::reserve(int64_t n) {
  DCHECK_GE(n, 0);
  const auto m = static_cast<size_t>(n);
  keys_.reserve(m);
  values_.reserve(m);
}

int64_t KeyValueMetadata::size() const {
  DCHECK_EQ(keys_.size(), values_.size());
  return static_cast<int64_t>(keys_.size());
}

std::string KeyValueMetadata::key(int64_t i) const {
  DCHECK_GE(i, 0);
  return keys_[static_cast<size_t>(i)];
}

std::string KeyValueMetadata::value(int64_t i) const {
  DCHECK_GE(i, 0);
  return values_[static_cast<size_t>(i)];
}

std::shared_ptr<KeyValueMetadata> KeyValueMetadata::Copy() const {
  return std::make_shared<KeyValueMetadata>(keys_, values_);
}

bool KeyValueMetadata::Equals(const KeyValueMetadata& other) const {
  return size() == other.size() &&
         std::equal(keys_.cbegin(), keys_.cend(), other.keys_.cbegin()) &&
         std::equal(values_.cbegin(), values_.cend(), other.values_.cbegin());
}
}  // namespace arrow
