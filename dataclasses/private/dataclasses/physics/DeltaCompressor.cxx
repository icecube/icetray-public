/* -------------------------------------------------------------------------------
 * Copyright (C) 2007
 * The IceCube Collaboration
 * @version $Id$
 *
 * @file DeltaCompressor.cxx
 * @author Martin Merck
 * @version $Revision$
 * @date $Date$
 * ------------------------------------------------------------------------------- 
 */
#include "dataclasses/physics/DeltaCompressor.h"
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cassert>

using namespace std;

namespace I3DeltaCompression
{
	/***************************************************************************
	 * Default Constructor
	 ***************************************************************************/
	DeltaCompressor::DeltaCompressor() :
		btw_(Lv2), offset_(0), currCompressedValue_(0)
	{
	}

	/***************************************************************************
	 * Add the vector of values to the compressed representation
	 ***************************************************************************/
	void DeltaCompressor::compress( const vector<int>& values )
	{
		vector<int>::const_iterator it;
		int lastVal = 0;
		
		currCompressedValue_ = 0;
		for( it = values.begin(); it < values.end(); it++)
		{
			int delta = *it - lastVal;
			lastVal = *it;
			compressDelta( delta );
		}
		return;
	}
	
	/***************************************************************************
	 * Decompress the values appending the waveform to the passed vector.
	 ***************************************************************************/
	void DeltaCompressor::decompress( vector<int>& values )
	{
		// Setup internal state
		btw_ = Lv2;
		offset_ = 0;
		
		// We store the last decompressed value in this variable,
		// to be able to reconstruce the next value based on the
		// decompressed delta.
		int lastValue = 0;
		
		// The variable datum will contain the comressed bits extracted
		// from the bitstream represented by the vector of  
		int datum     = 0;
		
		it_ = compressed_.begin();
		if( it_ ==  compressed_.end() )
			return;
		unsigned int data = *it_++;
		
        bool endCondition = false;
		while( !endCondition )
		{
			if( offset_ + btw_ <= 32 )
			{
				int mask = ( 1 << btw_ ) - 1;
				mask = mask << offset_;
				datum = ( data & mask) >> offset_;	

                if( (offset_ + btw_) == 32)
                {
                    if( it_ ==  compressed_.end() )
                        endCondition = true;
                    else
                        data = *it_++;
                }
			}
			else
			{
				if( it_ ==  compressed_.end() )
					break;
					
			    int mask = 0xFFFFFFFF << offset_;
				datum = ( data & mask) >> offset_;
				data = *it_++;
				int shift = ( btw_ - ( 32 - offset_ ) );
				mask = ( 0x01 << shift ) - 1;
				datum += ( data & mask) << ( btw_ - shift);		
        		
			} // if( offset_ + btw_ < 32 )
			
			int signMask = ( 1 << ( btw_ - 1 ) );
			offset_ += btw_;
			if( offset_ >= 32 )
				offset_ -= 32;
			
			// If the datum is a flag requesting a change in bitwidth,
			// we adjust the bitwidth and continue processing.
			if( datum == signMask )
			{
				btw_ = getNextBtw( btw_ );
				continue;
			}

			// make number negative if sign bit ia set.
			if( ( datum & signMask ) )
			{				
				datum |= ( 0xFFFFFFFF ^ (signMask - 1) );
			}
			
			lastValue += datum;
			values.push_back( lastValue );
			
			int prevBtw = getPrevBtw( btw_ );
			if( abs(datum) < ( 1 << ( prevBtw-1) ) )
				btw_ = prevBtw;
		
    } // while( true )
    
	} // DeltaCompressor::decompress( vector<int>& )
	
	/* -------------------------------------------------------------------------
	 * Intrenal helper to output a number of compressed bits 
	 * -------------------------------------------------------------------------
	 */
	void DeltaCompressor::compressDelta( int delta )
	{
		int maxVal = 1 << ( btw_ - 1 );
		
		// delta fits in current bitwidth
		if( abs(delta) < maxVal )
		{
		    // output data with current bitwidth
			outputBits( delta, btw_ );
			
			// check if delta was small enoug for previous bitwidth
			int prevMax = 1 << ( getPrevBtw( btw_ ) - 1 );
			if( abs(delta) < prevMax )
			{
				// delta is small enough, transition to a lower btw
				btw_ = getPrevBtw( btw_ );
				
			} // if( abs(delta) < prevMax )
		}
		else
		{
			// output flag
			outputBits(  1 << ( btw_ - 1 ), btw_ );
			
			// output data with longer bitwidth
			btw_ = getNextBtw( btw_ );
			compressDelta( delta );
			
		} // if( abs(delta) < maxVal )
		
	} // DeltaCompressor::compressDelta( int )

	// ------------------------------------------------------------------------------
	// private helper functions
	// ------------------------------------------------------------------------------
	
	//
	// Intrenal helper to output a number of compressed bits 
	//
	void DeltaCompressor::outputBits( int word, int len )
	{
                assert( len < 32 );
		// get a bitmask of the relevant bits
		int mask = ( 1 << len ) - 1;
		
		// check if data still fits in current int value of compression stream
		if( ( offset_ + len ) <= 32 )
		{
			int datum = ( word & mask ) << offset_;
			currCompressedValue_ += datum;

			// increase offset by the number of bits output.
			// if e filled the word, reset the offset ad get next word.
			offset_ += len;
			if( offset_ == 32 )
			{
				offset_ = 0;
				compressed_.push_back( currCompressedValue_ );
				currCompressedValue_ = 0;
				
			} // if( offset_ == sizeof( int ) )
		}
		else
		{
			// first output lower part of word.
			int part1 = 32 - offset_;
			outputBits( word, part1 );
			
			// reduce word to the upper part of the bits
			// and output the rest.
			word = word >> part1;
			len -= part1;
			outputBits( word, len );
			
		} // if( ( offset_ + len ) < sizeof( int ) )
		
	} // DeltaCompressor::outputBits( int, int )

	//
	// Intrenal helper to get next bitwidth value
	//
	int DeltaCompressor::getNextBtw( int btw ) const
	{
		switch ( btw )
		{
			case Lv0:
				return Lv1;
				break;
			case Lv1:
				return Lv2;
				break;
			case Lv2:
				return Lv3;
				break;
			case Lv3:
				return Lv4;
				break;
			case Lv4:
				return Lv5;
				break;
			case Lv5:
				return Lv6;
				break;
			default:
        throw std::domain_error( "Internal compressor ERROR. Unexpected bitwidth change." );
		} // switch (btw)
	
	} // DeltaCompressor::getNextBtw( int )
	
	//
	// Intrenal helper to get previous bitwidth value
	//
	int DeltaCompressor::getPrevBtw( int btw ) const
	{
		switch (btw)
		{
			case Lv6:
				return Lv5;
				break;
			case Lv5:
				return Lv4;
				break;
			case Lv4:
				return Lv3;
				break;
			case Lv3:
				return Lv2;
				break;
			case Lv2:
				return Lv1;
				break;
			case Lv1:
				return Lv0;
				break;
			case Lv0:
				return Lv0;
				break;
			default:
        throw std::domain_error( "Internal compressor ERROR. Unexpected bitwidth change." );				
		} // switch (btw)
		
	} // DeltaCompressor::getPrevBtw( int )

} // namespace I3DeltaCompression

