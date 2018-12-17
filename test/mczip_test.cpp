//============================================================================
// Name        : Comp.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include "tester.hpp"
#include <cassert>
#include <iostream>

int main(int argc, char **argv) {
  mczip::Tester tester(std::cout);

  for (int i = 1; i < argc; ++i) {
    tester.test(argv[i]);
  }

  return EXIT_SUCCESS;
}
