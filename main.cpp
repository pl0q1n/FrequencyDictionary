#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <string_view>
#include <map>
#include <fstream>
#include <chrono>
#include <cctype>

#include "mmap_file.h"

using InOutFiles = std::pair<std::string_view, std::string_view>;

struct DictionaryNode {
  std::string_view str;
  size_t freq;

  bool operator<(DictionaryNode& node) {
    if (freq == node.freq) {
      return str < node.str;
    }
    return freq > node.freq;
  }
};


InOutFiles parse_arguments(int argc, char** argv) {
  if (argc != 3) {
    throw std::runtime_error("Wrong number of arguments");
  }

  return { argv[1], argv[2] };
}

void run(int argc, char** argv) {
  std::unordered_map<std::string, size_t> dictionary;
  
  auto [input_file, output_file] = parse_arguments(argc, argv);
  auto mmaped_file = MmapFile(input_file);

  auto file_data = mmaped_file.get_file_data();

  char* start = file_data.data();
  char* end = file_data.data();
  char* eof = file_data.data() + file_data.size_bytes();
  while (end != eof) {
    if (isalpha(*end)) {
      //*end = tolower(*end);
      end++;
    }
    else {
      auto cur = std::string(start, end - start);
      //std::transform(cur.begin(), cur.end(), cur.begin(), [](unsigned char c) { return std::tolower(c); });
      if (auto it = dictionary.find(cur); it != dictionary.end()) {
        it->second++;
      } 
      else {
        dictionary.emplace_hint(it, std::move(cur), 1);
      }
      while (end != eof && !isalpha(*end))
        end++;
      start = end;
    }
  }

  std::vector<DictionaryNode> nodes;
  nodes.reserve(dictionary.size());
  
  for (auto& node: dictionary) {
    nodes.push_back({ node.first, node.second });
  }
  
  std::sort(nodes.begin(), nodes.end());

  std::ofstream out_file(output_file, std::ofstream::trunc);

  for (auto& node : nodes) {
    out_file << node.freq << " " << node.str << '\n';
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
