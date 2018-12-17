
#include "literal.hpp"
#include <iostream>

namespace mczip {
Literal::Literal(TagByte &tagByte)
    : mCount(0), mType(LITERAL_1_BYTE), mBytePtr(nullptr), mTagByte(tagByte) {}

void Literal::print(std::ostream &out) {
  out << (size_t)mCount << ":" << std::endl;
}

void Literal::reset() { mCount = 0; }

void Literal::write(unsigned char *&out) {

  mTagByte.write(TagByte::LITERAL, out);
  const uint32_t count_minus_1 = mCount - 1;
  if (count_minus_1 < 128) {
#ifdef DEBUG
    std::cout << "write " << (size_t)LITERAL_1_BYTE << ":" << mCount
              << std::endl;
#endif
    *out++ = (LITERAL_1_BYTE << 7) + count_minus_1;
  } else {
#ifdef DEBUG
    std::cout << "write " << (size_t)LITERAL_N_BYTE << ":" << mCount
              << std::endl;
#endif
    uint32_t tmpCount = count_minus_1;
    unsigned char *tmpOut = out + 1;
    uint8_t count = 0;
    while (tmpCount > 0) {
      *tmpOut++ = tmpCount & 0xFF;
      tmpCount >>= 8;
      ++count;
    }

    *out = (LITERAL_N_BYTE << 7) + count;
    out = tmpOut;
  }
}

void Literal::read(const uint8_t *&in) {
  const uint8_t marker = (uint8_t) * (in++);
  mType = (marker >> 7);
  mCount = marker & 127;

  switch (mType) {
  case LITERAL_1_BYTE:
    ++mCount;
#ifdef DEBUG
    std::cout << "read " << (size_t)mType << ":" << mCount << std::endl;
#endif
    break;
  case LITERAL_N_BYTE: {
    uint8_t byteCount = mCount;
    uint8_t shift = 0;
    mCount = 0;
    for (; byteCount > 0; --byteCount) {
      mCount += *in++ << shift;
      shift += 8;
    }
    ++mCount;
#ifdef DEBUG
    std::cout << "read " << (size_t)mType << ":" << mCount << std::endl;
#endif
  } break;
  }
}
} // namespace mczip
