/* -------------------------------------------------------------------------------
 * Copyright (C) 2007
 * The IceCube Collaboration
 * @version $Id$
 *
 * @file DeltaCompressor.h
 * @author Martin Merck
 * @version $Revision$
 * @date $Date$
 * ------------------------------------------------------------------------------- 
 */
#ifndef DELTACOMPRESSOR_H_INCLUDED
#define DELTACOMPRESSOR_H_INCLUDED

#include <vector>

namespace I3DeltaCompression
{
	/**
	 * @brief Compressor to perform Delta-Compression on series of int values.
	 * 
	 * Delta-Compression is used in IceCube to compress the digitized waveforms
	 * from the ATWDs and FADCs. These waveforms are contain 11 bit unsigned
	 * integers and the original algorithm compresses the deltas of single bins
	 * of the waveform with a maximal difference of 11 bits. In this implementtion
	 * the algorithm has been extended to allow to compress deltas of uo to 31-bits.
	 * For performance reasons, no sanity check is performed on the input values.
	 * If the compressor enconters uncompressable values or the compressed data
	 * can't be decompressed an exception is thrown!
	 *
	 * The DeltaCompressor can be used as follows:
	 *
	 * @code
	 * #include <iostream>
	 * #include <algorithm>
	 * #include <iterator>
	 * #include "dataclasses/physics/DeltaCompressor.h"
	 * ...
	 * {
	 *     I3DeltaCompression::DeltaCompressor compressor;
	 *     compressor.compress( values );
	 *     vector<unsigned int> compressed = compressor.getCompressed();
	 *
	 *     std::cout << "The compressed waveform is:" << std::endl;
	 *     std::copy( compressed.begin(),
	 *                compressed.end(),
	 *                std::ostrem_iterator<unsigned int>( std::cot, " " ) );
	 *
	 *     compressor.reset();    // only necessary if you want to decompress
	 *                            // a different waveform
	 *     vector<int> decompressed;
	 *     compressor.setCompressed( compressed );
	 *     compressor.decompress( decompressed );
	 *
	 *     std::cout << "The decompressed waveform is:" << std::endl;
	 *     std::copy( decompressed.begin(),
	 *                decompressed.end(),
	 *                std::ostrem_iterator<int>( std::cot, " " ) );
	 * }
	 * @endcode
	 *
	 * The asymmetry betwen the compress and decompress method is by design to reduce
	 * the amount of copying od STL vetors.
	 * The implementation of the compressor expects the compressed values to be 
	 * represented in the machines byteorder and will delivere a compressed stream
	 * of unsigned integers in the machines byteorder. If the compresssed values
	 * must be portable between machines, the external program using this class, 
	 * must take care of the correct byteorder.
	 */
	class DeltaCompressor
	{
		public:
	  /**
	   * Enumeration of the variable bitwidth used. 
	   */
	  enum BTW
	  {
	    Lv0 = 1,
	    Lv1 = 2,
	    Lv2 = 3,
	    Lv3 = 6,
	    Lv4 = 11,
	    Lv5 = 20,
	    Lv6 = 31,
	  };
	  
	public:
	  /**
	   * Default constructor.
	   * Initializes the bitwidth with Lv2.
	   */
	  DeltaCompressor();
	  
	  /**
	   * Compresses the waveform passed in the vector of values. The compressed
	   * waveform is stored internally and can be retrieved using the the getCompressed()
	   * method.
	   * The internal state of the compressor is not reset by this call. This allows
	   * to compress several waveforms to the same internal compressed bitstream, by
	   * calling compress several times.
	   * The rationale behind this design is to allow for an emulation of the DOMs which
	   * combine all waveforms to on compressed stream.
	   *
	   * @param values The vector with the waveform to be compressed.
	   * @exception A std::domain_error is thrown in case of internal compression error.
	   */
	  void compress( const std::vector<int>& values);
	  
	  /**
	   * Decompresses the internal compressed waveform and append the restored values
	   * to the vector passed by reference. If this vector already contains elements,
	   * the decompressed waveform will be appended.
	   * The decompressed wafeform will most likely have more values then the original
	   * waveform with the additional bins all having the same value as the last bin
	   * of the original waveform (deltas of 0). It is the responsability of the caller
	   * to truncate this vector to the correct size.
	   *
	   * @param values A reference to a vector to which the decompressed waveform will
	   *               be appended.
	   * @exception A std::domain_error is thrown in case of internal conpression error.
	   */
	  void decompress( std::vector<int>& values );
	  
	  /**
	   * Reset the internal state of the compressor by deleting the stored compressed
	   * waveform and resetting the variable bitwidth to the default value.
	   q*/
	  void reset()
	  {
	    offset_ = 0;
	    btw_ = Lv2;
	    compressed_.clear();
	  }
	  
	  /**
	   * Get a reference to the vector containing the compressed waveform.
	   *
	   * @return a const vector of unsigned integers representing the 
	   *         compressed bitstream.
	   */    
	  const std::vector<unsigned int>& getCompressed()
	  {
	      if( offset_ != 0)
		compressed_.push_back( currCompressedValue_ );
	    
	    offset_ = 0;
	    btw_ = Lv2;
	    
	    return compressed_; 
	  };
	  
	  /**
	   * Set the internal compressed waveform to the values of the passed vector.
	   * The waveform is coppied to the internal representation.
	   *
	   * @param vals const vector representing a compressed waveform.
	   */         
	  void setCompressed( const std::vector<unsigned int>& vals ) 
	  {
	    compressed_.clear();
	    compressed_.assign( vals.begin(), vals.end() );
	  }
	  
	 private:
	  
	  /**
	   * Utillity function to get the next higher bitwidth.
	   *
	   * @param btw current bitwidth
	   * @return next higher bitwidth
	   * @exception a std::domain_error is thrown when the function is called
	   *           with the highest btw or an invalid btw.
	   */
	  int getNextBtw( int btw ) const;
	  
	  /**
	   * Utillity function to get the next lower bitwidth.
	   *
	   * @param btw current bitwidth
	   * @return next lower bitwidth
	   * @exception a std::domain_error is thrown when the function is called
	   *           with the lowest btw or an invalid btw.
	   */
	  int getPrevBtw( int btw ) const;
	  
	  /**
	   * Write the compressed bits corresponding to a passed delta value.
	   *
	   * @param delta difference to previous bin to be written in compressed form.
	   */
	  void compressDelta( int delta );
	  
	  /**
	   * Output len bits of the word to the compressed bitstream.
	   *
	   * @param word Bits to write.
	   * @param len length of significant bits which should be written.
	   */
	  void outputBits( int word, int len );
	  
	private:
	  int btw_;
	  unsigned int offset_;
	  unsigned int currCompressedValue_;
	  std::vector<unsigned int> compressed_;
	  std::vector<unsigned int>::iterator it_;
	};
}

#endif // DELTACOMPRESSOR_H_INCLUDED

