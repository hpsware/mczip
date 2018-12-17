#ifndef MCZIP_LITERAL_HPP_
#define MCZIP_LITERAL_HPP_

#include "tagbyte.hpp"
#include <iostream>

namespace mczip {
class Literal {
public:
  enum LiteralType { LITERAL_1_BYTE, LITERAL_N_BYTE };

  Literal(TagByte &tagByte);
  void print(std::ostream &out);
  void reset();
  void write(unsigned char *&out);
  void read(const uint8_t *&in);

  uint32_t mCount;
  uint8_t mType;
  const uint8_t *mBytePtr;
  TagByte &mTagByte;
};
} // namespace mczip

#endif /* COMPRESSOR_HPP_ */
