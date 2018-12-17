#include "tuple.hpp"
#include <iostream>

namespace mczip {

void Tuple::readNumCopies(const uint8_t *&in) {
  if (mCount == mPrevCount && mOffset == mPrevOffset) {
    mNumCopies = *in++;

    if (mNumCopies >= Tuple::COPY_2_BYTES) {
      mNumCopies = ((mNumCopies & 127) << 8) + *in++;
    }
    mPrevCount = 0;
    mPrevOffset = 0;
  } else {
    mPrevCount = mCount;
    mPrevOffset = mOffset;
    mNumCopies = 1;
  }
}

void Tuple::read(const uint8_t *&in) {
  const uint8_t marker = (uint8_t) * (in++);
  mType = (marker >> 7);
  mCount = marker & 127;

  switch (mType) {
  case PATTERN_2_BYTE:
    mOffset = (mCount & 7) << 8;
    mOffset += *in++;
    mCount = (mCount & 120) >> 3;
    mCount += 4;
    mOffset += 4;
#ifdef DEBUG
    std::cout << "read " << (size_t)mType << ":" << mCount << ":" << mOffset
              << std::endl;
#endif
    break;
  case PATTERN_3_BYTE:
    mOffset = *in++;
    mOffset += *in++ << 8;
    mCount += 4;
    mOffset += 4;
#ifdef DEBUG
    std::cout << "read " << (size_t)mType << ":" << mCount << ":" << mOffset
              << std::endl;
#endif
    break;
  }

  readNumCopies(in);
}

void Tuple::write(unsigned char *&out, const uint8_t count) {

  const uint8_t len_minus_4 = count - 4;
  const uint32_t offset_minus_4 = mOffset - 4;

  mTagByte.write(TagByte::PATTERN, out);
  if (len_minus_4 < 16 && offset_minus_4 < 2048) {
#ifdef DEBUG
    std::cout << "write " << (size_t)PATTERN_2_BYTE << ":" << (size_t)count
              << ":" << mOffset << std::endl;
#endif
    *out++ = (PATTERN_2_BYTE << 7) + (len_minus_4 << 3) + (offset_minus_4 >> 8);
    *out++ = offset_minus_4 & 0xFF;
  } else {
#ifdef DEBUG
    std::cout << "write " << (size_t)PATTERN_3_BYTE << ":" << (size_t)count
              << ":" << mOffset << std::endl;
#endif
    *out++ = (PATTERN_3_BYTE << 7) + len_minus_4;
    *out++ = offset_minus_4 & 0xFF;
    *out++ = offset_minus_4 >> 8;
  }
  mCount = count;
}

void Tuple::writeFullTuple(unsigned char *&out) {
  mTagByte.write(TagByte::PATTERN, out);

#ifdef DEBUG
  std::cout << "write " << (size_t)PATTERN_3_BYTE << ":"
            << (size_t)MAX_PATTERN_SIZE << ":" << mOffset << std::endl;
#endif
  const uint8_t len_minus_4 = MAX_PATTERN_SIZE - 4;
  const uint32_t offset_minus_4 = mOffset - 4;
  *out++ = (PATTERN_3_BYTE << 7) + len_minus_4;
  *out++ = offset_minus_4 & 0xFF;
  *out++ = offset_minus_4 >> 8;
}

void Tuple::print(std::ostream &out) const {
  out << (size_t)mOffset << ":" << (size_t)mCount << ":" << std::endl;
}

Tuple::Tuple(TagByte &tagByte)
    : mCount(0), mOffset(0), mPrevCount(0), mPrevOffset(0),
      mType(PATTERN_2_BYTE), mNumCopies(0), mTagByte(tagByte) {}

} // namespace mczip
