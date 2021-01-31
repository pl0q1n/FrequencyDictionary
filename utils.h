#pragma once
#include <string_view>

namespace dirty_hacks {
  static inline char to_lower_lookup[] =
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107,
    108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0, 0, 0,
    0, 0, 0, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };
 
  static inline unsigned hash_fn_fnv(const char* buf, int len) {
    auto hash = 0x811C9DC5;
    while (len--)
    {
      hash = (hash * 0x01000193) ^ (unsigned int)*buf++;
    }
    return hash;
  }
}

static inline bool is_alpha(const char c) {
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ? 1 : 0);
}

static inline char to_lower(const char c) {
  return dirty_hacks::to_lower_lookup[c];
}

struct Key {
  std::string_view str;
  uint32_t hash;

  bool operator==(const Key& s) const 
  {
    int size = str.size();
    if (size != s.str.size()) {
      return false;
    }
    // this implementation of fnv hash doesn't create collisions on any
    // combinations of letters (a-z) with length that less than 6.
    // so if we want to check 2 keys from the same bucket
    // we could do it with little hack: we don't need check 
    // strings that have less than 6 symbols.
    if (size < 6) {
      return true;
    }

    return str == s.str;
  }

  Key(const std::string_view s, const uint32_t h) : str(s), hash(h) {}
};

struct KeyHasher {
  uint32_t operator()(const Key& key) const {
    return key.hash;
  }
};
