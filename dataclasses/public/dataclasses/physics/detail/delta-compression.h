/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Definition of deltacompression functions for mDOM/DEgg readout
 *
 * @file deltacompression.h
 * @date 2025-06-02
 * @author Schmidt
 * @author Stuttard
 *
 */

#ifndef DELTACOMPRESSION_H_INCLUDED
#define DELTACOMPRESSION_H_INCLUDED

#include <functional>
#include <set>
#include <vector>


namespace i3 { namespace dataclasses { namespace detail {
  namespace deltacompression {


/** Uncompress delta-compressed data such as a waveform from an xDOM.
 *
 * @param dest This is the destination of the uncompressed data. It MUST
 * have the correct size already, that is run \e dest.resize(numberOfSamples,0)
 * before calling \e uncompress. \e numberOfSamples is the expected
 * number of samples that \p source packs and it is not its size!
 * @param source This is a BLOB that contains the delta-compressed data.
 * @param startBitsPerWord This is the number of bits that the first delta/word
 * in \p source includes. Typically it is 3.
 * @param maxBitsPerWord The number of bits that a delta/word in \p source
 * includes changes during compression. This is the 'maximum bits per word',
 * so the 'bits per word' changes between one and \p maxBitsPerWord bits
 * (optional, default is 17).
 * @note Waveforms from an xDOM are little endian!!!
 */
void uncompress(std::vector<int>& dest, const std::vector<char>& source,
                unsigned int startBitsPerWord=3,
                unsigned int maxBitsPerWord=17);

/** Uncompress delta-compressed data such as a waveform from an xDOM.
 *
 * @param dest This is the destination of the uncompressed data. It MUST
 * have the correct size already, that is run \e dest.resize(numberOfSamples,0)
 * before calling \e uncompress. \e numberOfSamples is the expected
 * number of samples that \p source packs and it is not its size!
 * @param source This is a BLOB that contains the delta-compressed data.
 * @param sourceFirst It's possible that \p source does not only contain a
 * single waveform/BLOB, but many. The specific waveform/BLOB spans bins
 * [sourceFirst, (sourceFirst+sourceSize)[ in \p source.
 * @param sourceSize  It's possible that \p source does not only contain a
 * single waveform/BLOB, but many. The specific waveform/BLOB spans bins
 * [sourceFirst, (sourceFirst+sourceSize)[ in \p source.
 * @param startBitsPerWord This is the number of bits that the first delta/word
 * in \p source includes. Typically it is 3.
 * @param maxBitsPerWord The number of bits that a delta/word in \p source
 * includes changes during compression. This is the 'maximum bits per word',
 * so the 'bits per word' changes between one and \p maxBitsPerWord bits
 * (optional, default is 17).
 * @note Waveforms from an xDOM are little endian!!!
 */
void uncompress(std::vector<int>& dest,
                const std::vector<char>& source,
                unsigned int sourceFirst, unsigned int sourceSize,
                unsigned int startBitsPerWord,
                unsigned int maxBitsPerWord);

/** Uncompress delta-compressed data such as a waveform from a DOM or xDOM.
 *
 * @param dest This is the destination of the uncompressed data. It MUST
 * have the correct size already, that is run \e dest.resize(numberOfSamples,0)
 * before calling \e uncompress. \e numberOfSamples is the expected
 * number of samples that \p source packs and it is not its size!
 * @param source This is a BLOB that contains the delta-compressed data.
 * @param sourceFirst It's possible that \p source does not only contain a
 * single waveform/BLOB, but many. The specific waveform/BLOB spans bins
 * [sourceFirst, (sourceFirst+sourceSize)[ in \p source.
 * @param sourceSize  It's possible that \p source does not only contain a
 * single waveform/BLOB, but many. The specific waveform/BLOB spans bins
 * [sourceFirst, (sourceFirst+sourceSize)[ in \p source.
 * @param resetAt This variant of \e uncompress is targeted on traditional DOM
 * waveforms. In case of a traditional DOM \p source and \p dest contain the
 * samples of the FADC and the ATWD channels. At the 'waveform' boundaries
 * the \uncompress algorithm needs a 'reset'. \p resetAt contains the
 * indices/sample bins within \p dest where this reset will be executed such
 * as \code{.cpp} {128, 256, 256, 256} \endcode
 * @param startBitsPerWord This is the number of bits that the first delta/word
 * in \p source includes. Typically it is 3.
 * @param maxBitsPerWord The number of bits that a delta/word in \p source
 * includes changes during compression. This is the 'maximum bits per word',
 * so the 'bits per word' changes between one and \p maxBitsPerWord bits
 * (optional, default is 17).
 * @note Waveforms from a DOM or xDOM are little endian!!!
 */
void uncompress(std::vector<int>& dest,
                const std::vector<char>& source,
                unsigned int sourceFirst, unsigned int sourceSize,
                const std::set<unsigned int, std::less<unsigned int> >& resetAt,
                unsigned int startBitsPerWord,
                unsigned int maxBitsPerWord);

/** Compress data such as a waveform from an xDOM using delta-compression.
 *
 * @param dest This is the BLOB that will contain the delta-compressed data.
 * @param source This is the uncompressed data/waveform from an xDOM.
 * @param startBitsPerWord This is the number of bits that the first delta/word
 * in \p dest should include. Typically it is 3.
 * @param maxBitsPerWord The number of bits that a delta/word in \p dest will
 * include changes during compression. This is the 'maximum bits per word',
 * so the 'bits per word' changes between one and \p maxBitsPerWord bits
 * (optional, default is 17).
 * @note Waveforms from an xDOM are little endian!!!
 */
void compress(std::vector<char>& dest, const std::vector<int>& source,
              unsigned int startBitsPerWord=3,
              unsigned int maxBitsPerWord=17);


}  // namespace delta-compression
}  // namespace detail
}  // namespace dataclasses
}  // namespace i3

#endif  // DELTACOMPRESSION_H_INCLUDED


