#ifndef MCZIP_HASHTABLE_HPP_
#define MCZIP_HASHTABLE_HPP_

#include "tuple.hpp"

namespace mczip {
class HashTable {
public:
  /// Hash table size
  static const uint32_t MAX_NUM_ENTRIES = 4096 * 32;

  ~HashTable() {}

  HashTable() { clear(); }

  inline const uint8_t *&find(const uint8_t *const buffer) {
    return mEntries[hash(buffer)];
  }

  inline void clear() {
    memset(mEntries, 0, MAX_NUM_ENTRIES * sizeof(const uint8_t *));
  }

private:
  inline const uint32_t hash(const uint8_t *const buffer) const {
    const uint64_t result = -1123581321;
    return ((result * *(const uint64_t *const)buffer) >> 28) % MAX_NUM_ENTRIES;
  }

  const uint8_t *mEntries[MAX_NUM_ENTRIES];
};
} // namespace mczip

#endif /* COMPRESSOR_HPP_ */
