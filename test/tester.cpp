


#include "tester.hpp"
#include "stopwatch.hpp"

#include <cassert>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <string>

using namespace mczip;

Tester::Tester(std::ostream &output) : mOutput(output) {

  mOutput << std::left << std::setw(25) << "File" << std::left << std::setw(8)
          << "Test" << std::left << std::setw(10) << "Rate" << std::left << std::setw(15)
          << "Comp(MB/s)" << std::setw(15) << "Decomp(MB/s): " << std::endl;
}

bool Tester::test(const std::string &filename) {
  mTestPassed = true;

  mOutput << std::left << std::setw(25) << filename;
  loadFile(filename);

  std::string compressedBuffer = mCompressor.compress(mBuffer);
  checkBytes(mCompressor.decompress(compressedBuffer));

  mOutput << std::left << std::setw(10) << mCompressor.getCompressionRate();

  size_t numIterations = 0;
  Stopwatch watch;

  bool testPassed = true;

  do {
    mCompressor.compress(mBuffer);
    ++numIterations;
    watch.stop();
  } while (watch.getTimeDiffInSecondsWithHighresolution() < 1.0);

  mOutput << std::setw(15) << (numIterations * mBuffer.size()) / 1024 / 1024;

  numIterations = 0;
  watch.start();
  do {
    mCompressor.decompress(compressedBuffer);
    ++numIterations;
    watch.stop();
  } while (watch.getTimeDiffInSecondsWithHighresolution() < 1.0);

  mOutput << std::setw(15) << (numIterations * mBuffer.size()) / 1024 / 1024
          << std::endl;

  return mTestPassed;
}

bool Tester::checkBytes(std::string &processedBuffer) {
  assert(processedBuffer.size() == mBuffer.size());
  for (size_t i = 0; i < processedBuffer.size(); ++i) {
    if (mBuffer[i] != processedBuffer[i]) {
      mOutput << std::setw(8) << "FAILED";
      mTestPassed = false;
      return mTestPassed;
    } else {
    }
  }

  mOutput << std::setw(8) << "OK";
  return mTestPassed;
}

void Tester::loadFile(const std::string &fileName) {
  std::ifstream is(fileName.c_str(), std::ifstream::binary);

  assert(is.is_open() == true);
  // get mBuffer.size() of file:
  is.seekg(0, is.end);
  size_t length = is.tellg();
  is.seekg(0, is.beg);

  mBuffer.resize(length);

  // read data as a block:
  is.read(&mBuffer[0], mBuffer.size());

  is.close();
}

bool Tester::getHasPassed() const { return mTestPassed; }
