#ifndef TESTER_HPP_
#define TESTER_HPP_

#include "mczip.hpp"

#include <ostream>

namespace mczip {
class Tester {
public:
  Tester(std::ostream &output);
  bool test(const std::string &filename);
  bool getHasPassed() const;

private:
  bool checkBytes(std::string &processedBuffer);
  void loadFile(const std::string &fileName);

  std::ostream &mOutput;
  McZip mCompressor;
  std::string mBuffer;
  bool mTestPassed;
};
} // namespace mczip

#endif
