#include "mczip.hpp"
#include "zipper.hpp"

using namespace mczip;

McZip::McZip() : mZipper(new Zipper()) {}

McZip::~McZip() { delete mZipper; }

double McZip::getCompressionRate() const {
  return mZipper->getCompressionRate();
}

size_t McZip::getNumCompressedBytes() const {
  return mZipper->getNumCompressedBytes();
}

std::string &McZip::compress(const char *buffer, const uint64_t len) {
  return mZipper->compress(buffer, len);
}

std::string &McZip::compress(std::string &buffer) {
  return mZipper->compress(buffer.c_str(), buffer.size());
}

std::string &McZip::decompress(std::string &buffer) {
  return mZipper->decompress(buffer.c_str(), buffer.size());
}

std::string &McZip::decompress(const char *buffer, const uint64_t len) {
  return mZipper->decompress(buffer, len);
}
