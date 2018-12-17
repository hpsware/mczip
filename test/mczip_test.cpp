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

  tester.test("testdata/dickens");
  tester.test("testdata/mozilla");
  tester.test("testdata/mr");
  tester.test("testdata/nci");
  tester.test("testdata/ooffice");
  tester.test("testdata/osdb");
  tester.test("testdata/reymont");
  tester.test("testdata/samba");
  tester.test("testdata/sao");
  tester.test("testdata/webster");
  tester.test("testdata/x-ray");
  tester.test("testdata/xml");

  return EXIT_SUCCESS;
}
