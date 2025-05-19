#include <cstdlib>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "ERROR: Must specify input file" << std::endl;
    return EXIT_FAILURE;
  }

  std::fstream input(argv[1], std::ios::in | std::ios::binary);
  if (input.fail()) {
    std::cerr << "ERROR: Could not open input" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "#include \"frontends/pbrt/defaults_embed.h\"" << std::endl;
  std::cout << "const char kDefaults[] = {" << std::endl;

  char c = 0;
  while (input.get(c)) {
    std::cout << +c << "," << std::endl;
  }

  std::cout << "};" << std::endl;
  std::cout << "const size_t kDefaultsSize = sizeof(kDefaults);" << std::endl;

  return EXIT_SUCCESS;
}
