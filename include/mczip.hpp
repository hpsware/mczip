#ifndef MCZIP_CZIP_HPP_
#define MCZIP_CZIP_HPP_

#include <string>

namespace mczip {

class Zipper;

/**
 * This class serves as a compressor decompressor of data
 * streams. The underlying concept is based on LZ77. Main
 * target was to maximize performance by sacrifying compression
 * rate. Several parameters are used which can be customized by
 * potential users.
 *
 * @author Marco Czech
 */
class McZip {
public:
  McZip();
  std::string &compress(const char *buffer, const uint64_t len);
  std::string &compress(std::string &buffer);
  std::string &decompress(std::string &buffer);
  std::string &decompress(const char *buffer, const uint64_t len);

  virtual ~McZip();
  double getCompressionRate() const;
  size_t getNumCompressedBytes() const;

private:
  Zipper *mZipper;
};

} // namespace mczip

#endif
