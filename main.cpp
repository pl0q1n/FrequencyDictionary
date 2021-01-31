#include <cctype>
#include <fstream>
#include <chrono>

#include "app.h"


int main(int argc, const char** argv) {
  try {
    run(argc, argv);
  }
  catch (std::runtime_error& x) {
    std::cerr << x.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


