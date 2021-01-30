#include <cctype>
#include <fstream>
#include <chrono>

#include "app.h"


int main(int argc, const char** argv) {
  try {
    auto t1 = std::chrono::high_resolution_clock::now();
    run(argc, argv);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "miliseconds: " << std::chrono::duration_cast<std::chrono::milliseconds>((t2-t1)).count() << std::endl;
  }
  catch (std::runtime_error& x) {
    std::cout << x.what() << std::endl;
  }
}


