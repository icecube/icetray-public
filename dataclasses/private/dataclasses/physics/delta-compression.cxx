/**
 * Copyright  (C) 2025 the icecube collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <dataclasses/physics/detail/delta-compression.h>

#include <cmath>
#include <cstdint>
#include <sstream>
#include <stdexcept>

#include <boost/next_prior.hpp>
#include <boost/utility.hpp>

using std::less;
using std::ostringstream;
using std::set;
using std::uint32_t;
using std::vector;


//#define DELTA_COMPRESSION_DO_PRINT
#ifdef DELTA_COMPRESSION_DO_PRINT
#include <iomanip>
#include <iostream>
using std::cout;
using std::endl;
using std::hex;
using std::dec;
using std::setw;
using std::setfill;
#include <bitset>
using std::bitset;
#endif


namespace i3 { namespace dataclasses { namespace detail {
  namespace deltacompression {


/** Check whether a specific bit is set in a 'binary number'.
 * 
 * @param value This is the binary number to check. Could be an uint32_t
 * for instance.
 * @param bit Specifies the bit to check. For instance, 0 specifies the
 * LSB and 7 the MSB in a byte.
 * @return True, if the specific bit is set.
 */
template <class T>
bool getbit(T value, unsigned int bit) {
  return value & (0x01 << bit);
}


namespace uncompress_ns {


/** This is an interface to a blob to extract bit data from it.
 */
class BitBuffer {
 public:
  /** ctor.
   * 
   * @param buffer This is the blob that contains the bit data.
   * @param start This addresses the first element within \p buffer
   * that contains bit data that is later read from the bit buffer.
   * @param size This is the size (in bytes/chars) of the bit buffer
   * starting from \p start within \p buffer.
   */
  BitBuffer(const vector<char>& buffer,
            unsigned int start, unsigned int size)
    : buffer_(buffer),
      bytePosition_(start), bitPosition_(0u), 
      remainingBits_(size * 8u) {
    if(buffer.size() < start + size) {
      ostringstream oss;
      oss << "expected at least " << size << " bytes from this buffer, got "
          << buffer.size() - start << " instead";
      throw std::out_of_range(oss.str());
    }
  }

  /** Get the number of remaining bits in the buffer.
   * 
   * @return The remaining bits.
   */
  unsigned int GetRemainingBits() {
    return remainingBits_;
  }

  /** Read/get/remove the next \p bpw bits from the bit buffer.
   * 
   * @param bpw Bits per word, bits to read/get/remove.
   * As yet, \p bpw must not exceed 24 bits.
   * @return The (double) word from \p BitBuffer. The lower
   * \p bpw bits match the bits read/removed. The upper bits
   * are not set.
   */
  uint32_t Pop(unsigned int bpw) {
    if(bpw > 24u) {  // TODO: ok, we could have (32 - 1 - bitPosition_) here
      throw std::invalid_argument("bpw cannot exceed 24 bits");
    }
    if(bpw > remainingBits_) {
      throw std::out_of_range("bpw cannot exceed the remaining bits in buffer");
    }

    uint32_t mask = ((0x01 << bpw) - 1) << bitPosition_;
    // The delta compressed data is little endian so on a big endian
    // machine we need to endian swap the data:
    uint32_t retVal =
      *(reinterpret_cast<const uint32_t*>(boost::addressof(buffer_[bytePosition_])));
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    ;  // no-op
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    retVal = (((x & 0xff000000u) >> 24) |
              ((x & 0x00ff0000u) >>  8) |
              ((x & 0x0000ff00u) <<  8) |
              ((x & 0x000000ffu) << 24));
#else
#error Unable to determine machine endianness!
#endif
#ifdef DELTA_COMPRESSION_DO_PRINT
    bitset<32> tmpRetVal(retVal);
    cout << "BITBUFFER POP: " << tmpRetVal
         << " (bpw=" << bpw << ", " << "bitposition=" << bitPosition_ << ")"
         << endl;
#endif
    retVal = (retVal & mask) >> bitPosition_;
#ifdef DELTA_COMPRESSION_DO_PRINT
    tmpRetVal = retVal;
    cout << "BITBUFFER POP OUTPUT: " << tmpRetVal << endl;
#endif

    remainingBits_ -= bpw;
    bitPosition_ += bpw;
    unsigned int byteOffset = bitPosition_ / 8;
    bytePosition_ += byteOffset;
    bitPosition_ = bitPosition_ % 8;

    return retVal;
  }

 private:
  const vector<char>& buffer_;
  unsigned int bytePosition_;
  unsigned int bitPosition_;
  unsigned int remainingBits_;
};


/** This is a delta value (cf. delta compression).
 */
class Delta {
 public:
  static bool GoodBpw(unsigned int bpw) {
    return (bpw &&
            (validBpws_.find(bpw) != validBpws_.end()));
  }

  static void CheckBpw(unsigned int bpw, unsigned int maxBpw) {
    if (bpw > maxBpw) {
      throw std::invalid_argument("invalid 'bits per word', bpw > maxBpw");
    }
    if (!GoodBpw(bpw)) {
      throw std::invalid_argument("invalid 'bits per word', bpw");
    }
    if (!GoodBpw(maxBpw)) {
      throw std::invalid_argument("invalid 'maximum bits per word', maxBpw");
    }
  }

  static unsigned int GetNextLowerBpw(unsigned int bpw) {
    return *(boost::prior(validBpws_.find(bpw)));
  }

  static unsigned int GetNextHigherBpw(unsigned int bpw) {
    return *(boost::next(validBpws_.find(bpw)));
  }


  /** ctor.
   * 
   * @param value The raw/packed value. Only the lower \p bpw bits
   * are eventually set, the upper 32 - \p bpw bits must not be set.
   * @param bpw The 'bits per word' value of this delta value.
   * @param maxBpw The maximum 'bits per word' value used during
   * delta compression (optional, default is 17).
   */
  Delta(uint32_t value, unsigned int bpw, unsigned int maxBpw=17u) {
    CheckBpw(bpw, maxBpw);

    isFlag_ = (bpw == maxBpw) ? false : (value == (0x01 << (bpw - 1)));
    if (!isFlag_) {
      bool msb = getbit(value, bpw - 1);
      if (msb) {
        value_ = value - (0x01 << bpw);
      } else {
        value_ = value;
      }
      hasWastedBits_ = (bpw == 1) ?
        false : (std::abs(value_) < (0x01 << (Delta::GetNextLowerBpw(bpw) - 1)));
    }
   }

  /** Check whether this delta is a flag.
   * 
   * @return True, if it is a flag.
   */
  bool IsFlag() {
    return isFlag_;
  };

  /** Get the DECODED/unpacked delta value if it is not a flag.
   * 
   * @return The value.
   */
  int GetValue() {
    if (IsFlag()) {
      throw std::runtime_error("a flag has no value");
    }
    return value_;
  }

  /** Check whether a delta value has wasted bits.
   * 
   * @return True, if it has wasted bits.
   */
  bool HasWastedBits() {
    if (IsFlag()) {
      throw std::runtime_error("a flag has no wasted bits");
    }
    return hasWastedBits_;
  }

 private:
  static const set<unsigned int, less<unsigned int>> validBpws_;


  bool isFlag_;
  int value_;
  bool hasWastedBits_;
};


const set<unsigned int> Delta::validBpws_ = {1, 2, 3, 6, 11, 17};


}  // namespace uncompress_ns
namespace compress_ns {


/** This is an interface to a blob to insert bit data into it.
 */
class BitBuffer {
 public:
  /** ctor.
   * 
   * @param buffer This is the blob that will contain the bit data later on.
   */
  BitBuffer(vector<char>& buffer)
    : buffer_(buffer),
      bytePosition_(0u), bitPosition_(0u), remainingBits_(32u) {
    buffer_.resize(4, 0);
  }

  /** Read/get/remove the next \p bpw bits from the bit buffer.
   * 
   * @param bpw Bits per word, bits to read/get/remove.
   * As yet, \p bpw must not exceed 24 bits.
   * @return The (double) word from \p BitBuffer. The lower
   * \p bpw bits match the bits read/removed. The upper bits
   * are not set.
   */
  unsigned int Push(uint32_t bits, unsigned int bpw) {
    if(bpw > 24u) {  // TODO: ok, this could be increased but we want it safe for now
      throw std::invalid_argument("bpw cannot exceed 24 bits");
    }

    uint32_t mask = ((0x01 << bpw) - 1);
    uint32_t* buf =
      reinterpret_cast<uint32_t*>(boost::addressof(buffer_[bytePosition_]));
#ifdef DELTA_COMPRESSION_DO_PRINT
    bitset<32> tmpBuf(*buf);
    bitset<32> tmpBits(bits);
    cout << "BITBUFFER PUSH: " << tmpBuf
         << " (bpw=" << bpw << ", bits=" << tmpBits << ")"
         << endl;
#endif
    *buf = *buf | ((bits & mask) << bitPosition_);
#ifdef DELTA_COMPRESSION_DO_PRINT
    tmpBuf = *buf;
    cout << "BITBUFFER PUSH RESULT: " << tmpBuf << endl;
#endif

    remainingBits_ -= bpw;
    bitPosition_ += bpw;
    unsigned int byteOffset = bitPosition_ / 8;
    if (byteOffset) {
      bytePosition_ += byteOffset;
      buffer_.resize(buffer_.size() + byteOffset);
    }
    bitPosition_ = bitPosition_ % 8;
    return (bitPosition_ > 0) ? bytePosition_ + 1 : bytePosition_;
  }

 private:
  vector<char>& buffer_;
  unsigned int bytePosition_;
  unsigned int bitPosition_;
  unsigned int remainingBits_;
};


/** This is a delta value (cf. delta compression).
 */
class Delta {
 public:
  static unsigned int GetNextLowerBpw(unsigned int bpw) {
    return
      i3::dataclasses::detail::deltacompression::uncompress_ns::Delta::GetNextLowerBpw(bpw);
  }

  static unsigned int GetNextHigherBpw(unsigned int bpw) {
    return
      i3::dataclasses::detail::deltacompression::uncompress_ns::Delta::GetNextHigherBpw(bpw);
  }


  /** ctor.
   * 
   * @param value The delta value. The difference between two successive
   * 'samples'. For example zero should be subtracted from the first sample,
   * the first sample from the second and so on.
   * @param bpw The 'bits per word' value to be used for this delta value.
   * @param maxBpw The maximum 'bits per word' value used during
   * delta compression (optional, default is 17).
   */
  Delta(int value, unsigned int bpw, unsigned int maxBpw=17u) {
    i3::dataclasses::detail::deltacompression::uncompress_ns::Delta::CheckBpw(bpw, maxBpw);

    int absValue = std::abs(value);
    if ((absValue < (0x01 << (bpw - 1))) ||
        ((bpw == maxBpw) && (value == -(0x01 << (bpw - 1))))) {
      isFlag_ = false;
      if (value < 0) {
        value_ = value + (0x01 << bpw);
      } else {
        value_ = value;
      }
      hasWastedBits_ = (bpw == 1) ?
        false : (absValue < (0x01 << (Delta::GetNextLowerBpw(bpw) - 1)));
    } else if (bpw == maxBpw) {
      ostringstream oss;
      if (value < 0) {
        oss << "cannot pack, " << value << " < " << -(0x01 << (bpw - 1));
      } else {
        oss << "cannot pack, " << value << " >= " << (0x01 << (bpw - 1));
      }
      throw std::invalid_argument(oss.str());
    } else {
      isFlag_ = true;
      value_ = (0x01 << (bpw - 1));
      hasWastedBits_ = false;
    }
  }

  /** Check whether this delta is a flag.
   * 
   * @return True, if it is a flag.
   */
  bool IsFlag() {
    return isFlag_;
  };

  /** Get the ENCODED/packed delta value.
   * 
   * @return The value.
   */
  uint32_t GetValue() {
    return value_;
  }

  /** Check whether a delta value has wasted bits.
   * 
   * @return True, if it has wasted bits.
   */
  bool HasWastedBits() {
    return hasWastedBits_;
  }

 private:
  bool isFlag_;
  uint32_t value_;
  bool hasWastedBits_;
};


}  // namespace compress_ns


void uncompress(vector<int>& dest, const vector<char>& source,
                unsigned int startBitsPerWord, unsigned int maxBitsPerWord) {
  uncompress(dest, source, 0, source.size(), startBitsPerWord, maxBitsPerWord);
}


void uncompress(vector<int>& dest, const vector<char>& source,
                unsigned int sourceFirst, unsigned int sourceSize,
                unsigned int startBitsPerWord, unsigned int maxBitsPerWord) {
  uncompress(dest,
             source, sourceFirst, sourceSize,
             set<unsigned int, less<unsigned int> >(),
             startBitsPerWord, maxBitsPerWord);
}


void uncompress(vector<int>& dest, const vector<char>& source,
                unsigned int sourceFirst, unsigned int sourceSize,
                const set<unsigned int, less<unsigned int> >& resetAt,
                unsigned int startBitsPerWord, unsigned int maxBitsPerWord) {
  using uncompress_ns::BitBuffer;
  using uncompress_ns::Delta;

  BitBuffer bits(source, sourceFirst, sourceSize);
  set<unsigned int, less<unsigned int> >::const_iterator resetIter =
    resetAt.empty() ? resetAt.end() : resetAt.begin();
  unsigned int bitsPerWord = startBitsPerWord;
  int value = 0;
  size_t destBin = 0;

  while ((bits.GetRemainingBits() >= bitsPerWord) &&
          (destBin < dest.size())) {
    Delta d(bits.Pop(bitsPerWord), bitsPerWord, maxBitsPerWord);
    if (d.IsFlag()) {
      bitsPerWord = Delta::GetNextHigherBpw(bitsPerWord);
    } else {
      value += d.GetValue();
      dest[destBin++] = value;
      if (d.HasWastedBits()) {
        bitsPerWord = Delta::GetNextLowerBpw(bitsPerWord);
      }

      if((resetIter != resetAt.end()) && (destBin == *resetIter)) {
        ++resetIter;
        bitsPerWord = startBitsPerWord;
        value = 0;
      }
    }
  }
}


void compress(vector<char>& dest, const vector<int>& source,
              unsigned int startBitsPerWord, unsigned int maxBitsPerWord) {
  using compress_ns::BitBuffer;
  using compress_ns::Delta;

  BitBuffer bits(dest);
  unsigned int bitsPerWord = startBitsPerWord;
  vector<int>::const_iterator value = source.begin();
  vector<int>::const_iterator lastValue = source.end();
  unsigned int size = 0;
  while (value != source.end()) {
    Delta d(lastValue != source.end() ? *value - *lastValue : *value,
            bitsPerWord, maxBitsPerWord);
    size = bits.Push(d.GetValue(), bitsPerWord);
    if (d.IsFlag()) {
      bitsPerWord = Delta::GetNextHigherBpw(bitsPerWord);
    } else {
      lastValue = value++;
      if (d.HasWastedBits()) {
        bitsPerWord = Delta::GetNextLowerBpw(bitsPerWord);
      }
    }
  }
  if (size % 4) {
    dest.resize(((size / 4) + 1) * 4);
  } else {
    dest.resize(size);
  }
}


}  // namespace delta-compression
}  // namespace detail
}  // namespace dataclasses
}  // namespace i3

