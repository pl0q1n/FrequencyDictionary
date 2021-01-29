#include <cctype>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <string_view>
#include <map>
#include <fstream>
#include <chrono>

#include "fast_string.h"
#include "mmap_file.h"

#define GOTTA_GO_FAST

#ifdef GOTTA_GO_FAST
#include "absl/container/flat_hash_map.h"
using Map = absl::flat_hash_map<FastString, size_t, FastString::Hasher>;
#else
using Map = std::unordered_map<FastString, size_t, FastString::Hasher>;
#endif

using InOutFiles = std::pair<std::string_view, std::string_view>;

struct DictionaryNode {
  DictionaryNode(std::string_view v, size_t f) : str(v), freq(f) {}
  std::string_view str;
  size_t freq;

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

InOutFiles parse_arguments(int argc, char** argv) {
  if (argc != 3) {
    throw std::runtime_error("Wrong number of arguments");
  }

  return { argv[1], argv[2] };
}

void run(int argc, char** argv) {
  Map dictionary(13370);

  auto [input_file, output_file] = parse_arguments(argc, argv);
  auto mmaped_file = MmapFile(input_file);

  auto file_data = mmaped_file.get_file_data();

  char* start = file_data.data;
  char* end = file_data.data;
  char* eof = file_data.data + file_data.size;
  
  while (end != eof) {
    if (is_alpha(*end)) {
      end++;      
    }
    else {
      FastString cur(start, end);
      if (auto it = dictionary.find(cur); it != dictionary.end()) {
        it->second++;
      } 
      else {
        dictionary.emplace_hint(it, std::move(cur), 1);
      }
      while (end != eof && !is_alpha(*end))
        end++;
      start = end;
    }
  }

  std::vector<DictionaryNode> nodes;
  nodes.reserve(dictionary.size());
  
  for (auto& node: dictionary) {
    auto str = node.first.get_data();
    nodes.emplace_back(str, node.second);
  }
  
  std::sort(nodes.begin(), nodes.end());

  std::ofstream out_file(output_file, std::ofstream::trunc | std::ofstream::binary);

  for (auto& node : nodes) {
    out_file << node.freq;
    out_file.put(' ');
    for (auto c : node.str) {
      out_file.put(to_lower(c));
    }
    out_file.put('\n');
  }
}

int main(int argc, char** argv) {
  try {
    auto t1 = std::chrono::high_resolution_clock::now();
    run(argc, argv);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "nanoseconds: " << (t2 - t1).count() << std::endl;
    std::cout << "microseconds: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << std::endl;
    std::cout << "milliseconds: " << std::chrono::duration_cast<std::chrono::milliseconds >(t2 - t1).count() << std::endl;
  }
  catch (std::runtime_error& x) {
    std::cout << x.what() << std::endl;
  }
}
