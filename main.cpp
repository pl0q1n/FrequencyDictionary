#include <cctype>
#include <fstream>
#include <chrono>

#include "app.h"


int main(int argc, const char** argv) {
  try {
    run(argc, argv);
  }
  catch (std::runtime_error& x) {
    std::cout << x.what() << std::endl;
  }
}


