#ifndef MCZIP_TAGBYTE_HPP_
#define MCZIP_TAGBYTE_HPP_

#include <iostream>

namespace mczip {

class TagByte {
public:
  enum Type { PATTERN, LITERAL };

  TagByte();

  void reset();

  Type getCurrentTagType();
  void write(const Type type, uint8_t *&out);

  Type read(const uint8_t *&in);

private:
  uint8_t *mTagWritePtr;
  uint32_t mCurrentNumTags;
  const uint8_t *mTagReadPtr;
};
} // namespace mczip

#endif /* COMPRESSOR_HPP_ */
