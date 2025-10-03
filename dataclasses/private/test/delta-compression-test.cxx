// SPDX-FileCopyrightText: 2025 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <I3Test.h>

#include <algorithm>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

#include <dataclasses/physics/detail/delta-compression.h>


using i3::dataclasses::detail::deltacompression::uncompress;
using i3::dataclasses::detail::deltacompression::compress;

namespace {


void createUnpacked(std::vector<int>& values,
                    std::mt19937& generator,
                    unsigned int maxBitsPerWord,
                    int minValue=std::numeric_limits<int>::min(),
                    int maxValue=std::numeric_limits<int>::max()) {
  int val = 0;
  for (int& value : values) {
    int rangeFrom = std::max(-(0x01 << (maxBitsPerWord - 1)),
                             minValue - val);
    int rangeTo = std::min((0x01 << (maxBitsPerWord - 1)) - 1,
                           maxValue - val);
    std::uniform_int_distribution<int> distr(rangeFrom, rangeTo);
    val += distr(generator);
    value = val;
  }
}


}


TEST_GROUP(delta-compression);

TEST(XDOMExample) {
  std::vector<char> source{static_cast<char>(0xbb), 0x62, 0x01, 0x02,
                           0x0b, 0x00, 0x00, 0x00};
  std::vector<int> result{0x03, 0x02, 0x04, 0x09, 0x02c9};
  std::vector<int> unpacked(5, 0);
  std::vector<char> packed;
  uncompress(unpacked, source, 3, 17);
  ENSURE(unpacked == result, "uncompress is failing");
  compress(packed, unpacked, 3, 17);
  ENSURE(packed == source, "compress is failing");
}

TEST(DOMExample) {
  std::vector<char> source{0x04, 0x23, 0x11, 0x00,
                           static_cast<char>(0xe0), 0x71, 0x06, static_cast<char>(0x07)};
  std::vector<int> result{0x91, 0x92, 0x92, 0x91,
                          0x92, 0x92, 0x91, 0x91,
                          0x92, 0x92, 0x92, 0x92,
                          0x91, 0x91, 0x91};
  std::vector<int> unpacked(15, 0);
  std::vector<char> packed;
  uncompress(unpacked, source, 3, 11);
  ENSURE(unpacked == result, "uncompress is failing");
  compress(packed, unpacked, 3, 11);
  ENSURE(packed == source, "compress is failing");
}

TEST(XDOMRandomData) {
  std::random_device device;
  std::mt19937 generator(device());
  for (unsigned int i = 0; i < 10000; ++i) {
    std::vector<int> source(1000, 0);
    createUnpacked(source, generator, 17, -(0x01 << 23), (0x01 << 23) - 1);
    std::vector<char> packed;
    std::vector<int> unpacked(source.size(), 0);
    compress(packed, source, 3, 17);
    uncompress(unpacked, packed, 3, 17);
    ENSURE(unpacked == source, "compress->uncompress is failing");
  }
}
