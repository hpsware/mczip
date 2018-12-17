#include "zipper.hpp"
#include <stdio.h>

using namespace mczip;

Zipper::Zipper()
    : mLiteral(mTagByte), mTuple(mTagByte), mShift(32),
      mWorkingBufferPtr(new uint8_t[WORKING_BUFFER_SIZE]) {
  reset();
}

Zipper::~Zipper() { delete[] mWorkingBufferPtr; }

double Zipper::getCompressionRate() const {
  return mBufferSize / (double)(mCompressedBuffer.size());
}

size_t Zipper::getNumCompressedBytes() const {
  return mCompressedBuffer.size();
}

std::string &Zipper::compress(std::string &buffer) {
  return compress(buffer.c_str(), buffer.size());
}

void Zipper::flushLiteral(const uint8_t *&buffer) {
  if (mLiteral.mCount > 0) {
    if (mTuple.mNumCopies == 1) {
      flushPattern();
    }
    flushCopy(mTuple.mNumCopies);
    mLiteral.write(mCompressBufferPtr);
    memcpy(mCompressBufferPtr, mLiteral.mBytePtr, mLiteral.mCount);
    mCompressBufferPtr += mLiteral.mCount;
    mLiteral.mCount = 0;
  }

  mLiteral.mBytePtr = buffer;
}

std::string &Zipper::compress(const char *buffer, const uint64_t len) {
  reset();

  mBufferSize = len;
  uint64_t remainingBytes = mBufferSize;
  uint64_t worstCompressionSize = remainingBytes * 2;
  mCompressedBuffer.resize(worstCompressionSize);
  mCompressBufferPtr = (uint8_t *)&mCompressedBuffer[sizeof(uint64_t)];
  memcpy(&mCompressedBuffer[0], &mBufferSize, sizeof(uint64_t));

  const uint8_t *currentBufferPtr = (const uint8_t *)buffer;

  while (remainingBytes >= WORKING_BUFFER_SIZE) {
    memcpy(mWorkingBufferPtr, currentBufferPtr, WORKING_BUFFER_SIZE);
    compressBlock(mWorkingBufferPtr, WORKING_BUFFER_SIZE);
    currentBufferPtr += WORKING_BUFFER_SIZE;
    remainingBytes -= WORKING_BUFFER_SIZE;
  }

  compressBlock(currentBufferPtr, remainingBytes);

  const size_t compressedBufferSize =
      mCompressBufferPtr - (uint8_t *)&mCompressedBuffer[0];

  mCompressedBuffer.resize(compressedBufferSize);

  return mCompressedBuffer;
}

void Zipper::flushCopy(const uint16_t numCopies) {
  if (numCopies > 0) {
#ifdef DEBUG_PATTERN_COPY
    static size_t count = 0;
    std::cout << "write " << ++count << " pattern copy: " << numCopies
              << std::endl;
#endif
    if (numCopies <= 127) {
      *mCompressBufferPtr++ = numCopies;
    } else {
      *mCompressBufferPtr++ = (1 << 7) + (numCopies >> 8);
      *mCompressBufferPtr++ = numCopies & 0xFF;
    }

    mTuple.mNumCopies = 0;
  }
}

void Zipper::flushPattern() {
  if (mTuple.mNumCopies <= 1) {
    uint32_t count = mTuple.mCount;
    uint16_t numFullTuples =
        (count - Tuple::MIN_PATTERN_SIZE) / (Tuple::MAX_PATTERN_SIZE);

    if (numFullTuples == 1) {
      mTuple.writeFullTuple(mCompressBufferPtr);
    } else if (numFullTuples > 1) {
      mTuple.writeFullTuple(mCompressBufferPtr);
      mTuple.writeFullTuple(mCompressBufferPtr);
      flushCopy(numFullTuples - 1);
    }

    count -= (Tuple::MAX_PATTERN_SIZE * numFullTuples);

    if (count >= Tuple::MAX_PATTERN_SIZE) {
      mTuple.write(mCompressBufferPtr,
                   Tuple::MAX_PATTERN_SIZE - Tuple::MIN_PATTERN_SIZE);
      count -= Tuple::MAX_PATTERN_SIZE - Tuple::MIN_PATTERN_SIZE;
    }

    if (count > 0) {
      mTuple.write(mCompressBufferPtr, count);
    }
  }
}

std::string &Zipper::decompress(const char *buffer, const uint64_t len) {
  uint64_t numUncompressedBytes = 0;

  memcpy(&numUncompressedBytes, buffer, sizeof(uint64_t));

  TagByte tagByte;
  Tuple tuple(tagByte);
  Literal literal(tagByte);

  const uint8_t *readPos = (const uint8_t *)(buffer + sizeof(uint64_t));
  const uint8_t *endPos = (const uint8_t *)(buffer + len);

  mUncompressedBuffer.resize(numUncompressedBytes);
  char *outPos = &mUncompressedBuffer[0];

  while (readPos < endPos) {
    const TagByte::Type type = tagByte.read(readPos);
    switch (type) {
    case TagByte::LITERAL:
      literal.read(readPos);
      memcpy(outPos, readPos, literal.mCount);
      outPos += literal.mCount;
      readPos += literal.mCount;
      tuple.reset();
      break;
    case TagByte::PATTERN:
      tuple.read(readPos);
      for (uint16_t i = 0; i < tuple.mNumCopies; ++i) {
        memcpy(outPos, outPos - tuple.mOffset, tuple.mCount);
        outPos += tuple.mCount;
      }
      break;
    }
  }
  return mUncompressedBuffer;
}

std::string &Zipper::decompress(std::string &buffer) {
  return decompress(buffer.c_str(), buffer.size());
}

bool Zipper::findPattern(const uint8_t *buffer, const uint8_t *entry,
                         uint64_t patternLen) {
  if (entry != 0 && *(const uint32_t *)entry == *(const uint32_t *)buffer) {
    const uint64_t offset = buffer - entry;

    if (offset <= MAX_WINDOW_SIZE && offset >= Tuple::MIN_PATTERN_SIZE) {
      uint32_t foundPatternLen = Tuple::MIN_PATTERN_SIZE;

      if (offset > Tuple::MIN_PATTERN_SIZE) {
        patternLen =
            std::min<uint64_t>(patternLen, offset) - Tuple::MIN_PATTERN_SIZE;
        foundPatternLen +=
            findPatternLen(entry + Tuple::MIN_PATTERN_SIZE,
                           buffer + Tuple::MIN_PATTERN_SIZE, patternLen);
      }

      if (foundPatternLen <= Tuple::MAX_PATTERN_SIZE &&
          mTuple.mOffset == offset && mTuple.mCount == foundPatternLen) {
        ++mTuple.mNumCopies;
      } else {
        flushCopy(mTuple.mNumCopies);
        mTuple.mOffset = offset;
        mTuple.mCount = foundPatternLen;
      }
      return true;
    }
  }

  return false;
}

void Zipper::compressBlock(const uint8_t *buffer, uint64_t len) {
  mHashTable.clear();
  mTuple.reset();
  flushLiteral(buffer);
  while (len >= Tuple::MIN_PATTERN_SIZE) {
    const uint8_t *&entry = mHashTable.find(buffer);
    const bool patternFound = findPattern(buffer, entry, len);
    entry = buffer;

    if (patternFound == false) {
      mTuple.mCount = 0;
      mTuple.mOffset = 0;
      const uint32_t skipBytes = std::min<uint32_t>(++mShift >> 5, len);
      mLiteral.mCount += skipBytes;
      buffer += skipBytes;
      len -= skipBytes;
    } else {
      buffer += mTuple.mCount;
      len -= mTuple.mCount;
      mShift = 32;
      flushLiteral(buffer);
      flushPattern();
    }
  }
  mLiteral.mCount += len;
  flushLiteral(buffer);
  flushCopy(mTuple.mNumCopies);
}

inline const uint32_t Zipper::findPatternLen(const uint8_t *window,
                                             const uint8_t *buffer,
                                             uint64_t len) const {
  uint32_t patternLen = 0;

  for (; len >= sizeof(uint64_t); len -= sizeof(uint64_t)) {
    if (*(const uint64_t *const)buffer != *(const uint64_t *const)window) {
      break;
    }
    patternLen += sizeof(uint64_t);
    buffer += sizeof(uint64_t);
    window += sizeof(uint64_t);
  }

  if (len >= sizeof(uint64_t)) {
    const uint64_t x = *(const uint64_t *)buffer ^ *(const uint64_t *)window;
    const int matching_bits = __builtin_ctzll(x);
    patternLen += matching_bits >> 3;
  } else {
    for (; len > 0; --len) {
      if (*buffer != *window) {
        break;
      }
      ++patternLen;
      ++buffer;
      ++window;
    }
  }

  return patternLen;
}

void Zipper::reset() {
  mBufferSize = 0;
  mShift = 0;
  mTagByte.reset();
}
