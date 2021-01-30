#pragma once
#include "fast_string.h"
#include "mmap_file.h"

#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <string_view>

#define GOTTA_GO_FAST

#ifdef GOTTA_GO_FAST
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
#include "absl/container/flat_hash_map.h"
using Map = absl::flat_hash_map<std::string_view, int, FancyHasher>;
#else
using Map = std::unordered_map<std::string_view, int, FancyHasher>;
#endif

using InOutFiles = std::pair<std::string_view, std::string_view>;

struct Key {
  std::string_view str;
  uint64_t hash;

  Key(std::string_view s, uint64_t h) : str(s), hash(h) {}
};

struct KeyHasher {
  uint64_t operator()(const Key& key) {
    return key.hash;
  }
};

struct DictionaryNode {
  DictionaryNode(std::string_view v, int f) : str(v), freq(f) {}
  std::string_view str;
  int freq;

  bool operator<(const DictionaryNode& node) {
    if (freq == node.freq) {
      return str < node.str;
    }
    return freq > node.freq;
  }
};

bool is_alpha(const char c) {
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ? 1 : 0);
}

InOutFiles parse_arguments(int argc, const char** argv) {
  if (argc != 3) {
    throw std::runtime_error("Wrong number of arguments");
  }

  return { argv[1], argv[2] };
}

void run(int argc, const char** argv) {
  Map dictionary(13370);

  const auto [input_file, output_file] = parse_arguments(argc, argv);
  auto mmaped_file = MmapFile(input_file);

  auto file_data = mmaped_file.get_file_data();

  char* start = file_data.data;
  char* end = file_data.data;
  const char* eof = file_data.data + file_data.size;
  
  // computer science dudes love to put file signatures into files, so we need to skip it. 
  while (!is_alpha(*start)) {
    start++;
  }
  end = start;

  while (end != eof) {
    if (is_alpha(*end)) {
      *end = to_lower(*end);
      end++;
      continue;
    }

    const auto str = std::string_view(start, end - start);
    
    // for some reason, dictionary.find + insert is faster than dictionary[str]++ or find + emplace_hint
    // in this case, find + insert will trigger double hash calculating, so we should eluminate this overhead.
    auto it = dictionary.find(str);
    if (it == dictionary.end()) {
      it = dictionary.insert(it, { str, 1 });
    }
    else {
      it->second++;
    }

    while (end != eof && !is_alpha(*end))
      end++;
    start = end;
  }

  std::vector<DictionaryNode> nodes;
  nodes.reserve(dictionary.size());

  for (auto& node : dictionary) {
    nodes.emplace_back(node.first, node.second);
  }

  // std::cout << dictionary.load_factor() << std::endl;
  std::sort(nodes.begin(), nodes.end());
  std::ofstream out_file(output_file, std::ofstream::trunc | std::ofstream::binary);

  for (const auto& n : nodes) {
    out_file << n.freq << ' ' << n.str << '\n';
  }
}
