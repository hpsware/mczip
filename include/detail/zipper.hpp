#ifndef ZIPPER_MCZIP_HPP_
#define ZIPPER_MCZIP_HPP_

#include <string>

#include "hashtable.hpp"
#include "literal.hpp"
#include "tuple.hpp"

namespace mczip {

/**
 * This class serves as a compressor decompressor of data
 * streams. The underlying concept is based on LZ77. Main
 * target was to maximize performance by sacrifying compression
 * rate. Several parameters are used which can be customized by
 * potential users.
 *
 * @author High Performance Software GmbH
 */
class Zipper {
public:
  Zipper();
  std::string &compress(const char *buffer, const uint64_t len);
  std::string &compress(std::string &buffer);
  std::string &decompress(std::string &buffer);
  std::string &decompress(const char *buffer, const uint64_t len);

  virtual ~Zipper();
  double getCompressionRate() const;
  size_t getNumCompressedBytes() const;

private:
  /// Sliding window size of compression
  static const uint32_t MAX_WINDOW_SIZE = 65539;
  /// Size of working buffer to operate in cache
  static const uint32_t WORKING_BUFFER_SIZE = 4096 * 256;

  void reset();
  void compressBlock(const uint8_t *buffer, uint64_t len);
  bool findPattern(const uint8_t *buffer, const uint8_t *entry,
                   uint64_t patternLen);
  const uint32_t findPatternLen(const uint8_t *window,
                                const uint8_t *const buffer,
                                uint64_t len) const;
  void flushLiteral(const uint8_t *&buffer);
  void flushPattern();
  void flushCopy(const uint16_t numCopies);

  std::string mCompressedBuffer;
  std::string mUncompressedBuffer;
  uint8_t *mCompressBufferPtr;
  Literal mLiteral;
  Tuple mTuple;
  HashTable mHashTable;
  size_t mBufferSize;
  uint32_t mShift;
  uint8_t *mWorkingBufferPtr;
  TagByte mTagByte;
};

} // namespace mczip

#endif /* ZIPPER_MCZIP_HPP_ */
