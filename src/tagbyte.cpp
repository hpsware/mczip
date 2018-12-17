#include "tagbyte.hpp"

namespace mczip {

TagByte::TagByte()
    : mTagWritePtr(nullptr), mCurrentNumTags(0), mTagReadPtr(nullptr) {}

void TagByte::reset() { mCurrentNumTags = 0; }

TagByte::Type TagByte::getCurrentTagType() {
  return (Type)(*mTagReadPtr >> mCurrentNumTags++ & LITERAL);
}

void TagByte::write(const TagByte::Type type, uint8_t *&out) {
  if (mCurrentNumTags != 0) {
    *mTagWritePtr |= (type << mCurrentNumTags);
    ++mCurrentNumTags %= 8;
  } else {
    mTagWritePtr = out++;
    *mTagWritePtr = type;
    mCurrentNumTags = 1;
  }
}

TagByte::Type TagByte::read(const uint8_t *&in) {
  mCurrentNumTags %= 8;

  if (mCurrentNumTags == 0) {
    mTagReadPtr = in++;
#ifdef DEBUG_TAG_HANDLING
    std::cout << "read " << (uint16_t)*mTagReadPtr << ":" << std::endl;
#endif
  }

  return getCurrentTagType();
}
} // namespace mczip
