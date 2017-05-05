/** @file

  A brief file description

  @section license License

  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

#ifndef CACHE_DEFS_H
#define CACHE_DEFS_H
#include <ts/I_Version.h>
#include <ts/Scalar.h>
#include <netinet/in.h>
#include <ts/Regex.h>
#include <ts/MemView.h>
#include "tsconfig/Errata.h"
#include <iostream>
namespace tag
{
struct bytes {
  static constexpr char const *const label = " bytes";
};
}

using namespace ApacheTrafficServer;

namespace ts
{
constexpr static uint8_t CACHE_DB_MAJOR_VERSION = 24;
/// Maximum allowed volume index.
constexpr static int MAX_VOLUME_IDX          = 255;
constexpr static int ENTRIES_PER_BUCKET      = 4;
constexpr static int MAX_BUCKETS_PER_SEGMENT = (1 << 16) / ENTRIES_PER_BUCKET;

typedef Scalar<1, off_t, tag::bytes> Bytes;
typedef Scalar<1024, off_t, tag::bytes> Kilobytes;
typedef Scalar<1024 * Kilobytes::SCALE, off_t, tag::bytes> Megabytes;
typedef Scalar<1024 * Megabytes::SCALE, off_t, tag::bytes> Gigabytes;
typedef Scalar<1024 * Gigabytes::SCALE, off_t, tag::bytes> Terabytes;

// Units of allocation for stripes.
typedef Scalar<128 * Megabytes::SCALE, int64_t, tag::bytes> CacheStripeBlocks;
// Size measurement of cache storage.
// Also size of meta data storage units.
typedef Scalar<8 * Kilobytes::SCALE, int64_t, tag::bytes> CacheStoreBlocks;
// Size unit for content stored in cache.
typedef Scalar<512, int64_t, tag::bytes> CacheDataBlocks;

/** A cache span is a representation of raw storage.
    It corresponds to a raw disk, disk partition, file, or directory.
 */
class CacheSpan
{
public:
  /// Default offset of start of data in a span.
  /// @internal I think this is done to avoid collisions with partition tracking mechanisms.
  static const Bytes OFFSET;
};

/** A section of storage in a span, used to contain a stripe.

    This is stored in the span header to describe the stripes in the span.

    @note Serializable.

    @internal nee @c DiskVolBlock
 */
struct CacheStripeDescriptor {
  Bytes offset;         // offset of start of stripe from start of span.
  CacheStoreBlocks len; // length of block.
  uint32_t vol_idx;     ///< If in use, the volume index.
  unsigned int type : 3;
  unsigned int free : 1;
};

/** Header data for a span.

    This is the serializable descriptor stored in a span.

    @internal nee DiskHeader
 */
struct SpanHeader {
  static constexpr uint32_t MAGIC = 0xABCD1237;
  uint32_t magic;
  uint32_t num_volumes;      /* number of discrete volumes (DiskVol) */
  uint32_t num_free;         /* number of disk volume blocks free */
  uint32_t num_used;         /* number of disk volume blocks in use */
  uint32_t num_diskvol_blks; /* number of disk volume blocks */
  CacheStoreBlocks num_blocks;
  /// Serialized stripe descriptors. This is treated as a variable sized array.
  CacheStripeDescriptor stripes[1];
};

/** Stripe data, serialized format.

    @internal nee VolHeadFooter
 */
// the counterpart of this structure in ATS is called VolHeaderFooter
class StripeMeta
{
public:
  static constexpr uint32_t MAGIC = 0xF1D0F00D;

  uint32_t magic;
  VersionNumber version;
  time_t create_time;
  off_t write_pos;
  off_t last_write_pos;
  off_t agg_pos;
  uint32_t generation; // token generation (vary), this cannot be 0
  uint32_t phase;
  uint32_t cycle;
  uint32_t sync_serial;
  uint32_t write_serial;
  uint32_t dirty;
  uint32_t sector_size;
  uint32_t unused; // pad out to 8 byte boundary
  uint16_t freelist[1];
};

/*
 @internal struct Dir in P_CacheDir.h
 * size: 10bytes
 */

class CacheDirEntry
{
public:
#if 0
  unsigned int offset : 24;
  unsigned int big : 2;
  unsigned int size : 6;
  unsigned int tag : 12;
  unsigned int phase : 1;
  unsigned int head : 1;
  unsigned int pinnned : 1;
  unsigned int token : 1;
  unsigned int next : 16;
  uint16_t offset_high;
#else
  uint16_t w[5];
#endif
};

class CacheVolume
{
};

class URLparser
{
public:
  bool verifyURL(std::string &url1);
  Errata parseURL(StringView URI);
  int getPort(std::string &fullURL, int &port_ptr, int &port_len);

private:
  //   DFA regex;
};

class CacheURL
{
public:
  in_port_t port;
  std::string scheme;
  std::string url;
  std::string hostname;
  std::string path;
  std::string query;
  std::string params;
  std::string fragments;
  std::string user;
  std::string password;
  CacheURL(int port_, ts::StringView b_hostname, ts::StringView b_path, ts::StringView b_params, ts::StringView b_query,
           ts::StringView b_fragments)
  {
    hostname.assign(b_hostname.begin(), b_hostname.size());
    port = port_;
    path.assign(b_path.begin(), b_path.size());
    params.assign(b_params.begin(), b_params.size());
    query.assign(b_query.begin(), b_query.size());
    fragments.assign(b_fragments.begin(), b_fragments.size());
  }

  CacheURL(ts::StringView blob, int port_)
  {
    url.assign(blob.begin(), blob.size());
    port = port_;
  }

  void
  setCredential(char *p_user, int user_len, char *p_pass, int pass_len)
  {
    user.assign(p_user, user_len);
    password.assign(p_pass, pass_len);
  }
};
}

class DFA;
// this class matches url of the format : scheme://hostname:port/path;params?query
struct url_matcher {
  // R"(^https?\:\/\/^[a-z A-Z 0-9]\.[a-z A-Z 0-9 \.]+)"
  url_matcher()
  {
    /*if (regex.compile(R"(^https?\:\/\/^[a-z A-Z 0-9][\. a-z A-Z 0-9 ]+(\:[0-9]\/)?.*))") != 0) {
        std::cout<<"Check your regular expression"<<std::endl;
    }*/
    //  (\w+\:[\w\W]+\@)? (:[0-9]+)?(\/.*)
    if (regex.compile(R"(^(https?\:\/\/)") != 0) {
      std::cout << "Check your regular expression" << std::endl;
      return;
    }
    if (port.compile(R"([0-9]+$)") != 0) {
      std::cout << "Check your regular expression" << std::endl;
      return;
    }
  }

  ~url_matcher() {}
  uint8_t
  match(const char *hostname) const
  {
    if (regex.match(hostname) != -1)
      return 1;
    //   if(url_with_user.match(hostname) != -1)
    //       return 2;
    return 0;
  }
  uint8_t
  portmatch(const char *hostname, int length) const
  {
    if (port.match(hostname, length) != -1)
      return 1;
    //   if(url_with_user.match(hostname) != -1)
    //       return 2;
    return 0;
  }

private:
  DFA port;
  DFA regex;
};

#endif // CACHE_DEFS_H
