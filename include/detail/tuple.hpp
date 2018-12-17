#ifndef MCZIP_TUPLE_HPP_
#define MCZIP_TUPLE_HPP_

#include "tagbyte.hpp"
#include <iostream>

namespace mczip {
class Tuple {
public:
  static const uint8_t MAX_PATTERN_SIZE = 131;
  static const uint8_t MIN_PATTERN_SIZE = 4;
  /// Code to indicate 2 bytes were used for copy count
  static const uint8_t COPY_2_BYTES = 128;

  enum PatternType {
    PATTERN_2_BYTE,
    PATTERN_3_BYTE,
  };

  Tuple(TagByte &tagByte);

  inline void reset() {
    mCount = 0;
    mOffset = 0;
    mNumCopies = 0;
    mPrevOffset = 0;
    mPrevCount = 0;
  }

  void print(std::ostream &out) const;
  void write(unsigned char *&out, const uint8_t count);
  void writeFullTuple(unsigned char *&out);
  void read(const uint8_t *&in);
  void readNumCopies(const uint8_t *&in);

  uint32_t mCount;
  uint32_t mOffset;
  uint32_t mPrevCount;
  uint32_t mPrevOffset;
  uint8_t mType;
  uint16_t mNumCopies;
  TagByte &mTagByte;
};
} // namespace mczip

#endif /* COMPRESSOR_HPP_ */
