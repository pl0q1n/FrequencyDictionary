#pragma once
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <string_view>

#include "utils.h"
#include "mmap_file.h"

#ifdef GOTTA_GO_FAST
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
#include "3rdParty/pqsort.h"
#include "absl/container/flat_hash_map.h"
using Map = absl::flat_hash_map<Key, uint32_t, KeyHasher>;
#else
using Map = std::unordered_map<Key, uint32_t, KeyHasher>;
#endif

using InOutFiles = std::pair<std::string_view, std::string_view>;

struct DictionaryNode {
  DictionaryNode(const std::string_view v, const uint32_t f) : str(v), freq(f) {}
  std::string_view str;
  int freq;

  bool operator<(const DictionaryNode& node) const {
    if (freq == node.freq) {
      return str < node.str;
    }
    return freq > node.freq;
  }
};


InOutFiles parse_arguments(int argc, const char** argv) {
  if (argc != 3) {
    throw std::runtime_error("Please use following pattern: FreqDictionary *input* *output*");
  }
  return { argv[1], argv[2] };
}

void run(int argc, const char** argv) {
  Map dictionary(16383);
  const auto [input_file, output_file] = parse_arguments(argc, argv);
  auto mmaped_file = MmapFile(input_file);

  // open it and test before processing to not waste time
  std::ofstream out_file(output_file, std::ofstream::trunc | std::ofstream::binary);
  if (out_file.bad()) {
    throw std::runtime_error("Can't open output file for writing!");
  }


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

    auto count = end - start;
    auto str = std::string_view(start, count);
    
    const Key key(str, dirty_hacks::hash_fn_fnv(start, count));
    // for some reason, dictionary.find + insert is faster than dictionary[str]++ or find + emplace_hint
    // in this case, find + insert will trigger double hash calculating, so we should eluminate this overhead.
    auto it = dictionary.find(key);
    if (it == dictionary.end()) {
      it = dictionary.insert(it, { key, 1});
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
    nodes.emplace_back(node.first.str, node.second);
  }

#ifdef GOTTA_GO_FAST
  pdqsort(nodes.begin(), nodes.end());
#else
  std::sort(nodes.begin(), nodes.end());
#endif

  for (const auto& n : nodes) {
    out_file << n.freq << ' ' << n.str << '\n';
  }
}
