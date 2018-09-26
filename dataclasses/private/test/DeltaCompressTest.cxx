/**
 * copyright  (C) 2007
 * the icecube collaboration
 *
 * Test Delta Compressions ability to encode and decode 
 *
 * @file    dataclasses/private/test/DeltaCompressTest.cxx
 * @version $Revision$
 * @date    $Date$
 * @author  Kevin Meagher
 */
#include <I3Test.h>
#include <../private/dataclasses/physics/DeltaCompressor.h>

using namespace std;

TEST_GROUP(DeltaCompressionTest)


TEST(RandomWaveTest)
{

  for (int trial=0; trial < 1000 ;trial++)
    {

      int waveform_constant=RAND_MAX/128;

      vector<int> orig_waveform(128,0);
      
      for (int i=0;i < 128; i++)
	{
	  orig_waveform[i]=rand()/waveform_constant;
	}

      I3DeltaCompression::DeltaCompressor compressor;
      compressor.reset();
      compressor.compress(orig_waveform);
      const vector<unsigned int>& compressed_waveform = compressor.getCompressed();

      I3DeltaCompression::DeltaCompressor uncompressor;
      uncompressor.reset();
      uncompressor.setCompressed( compressed_waveform );
      vector<int> new_waveform;
      new_waveform.clear();
      uncompressor.decompress( new_waveform );
      /*
      Delta decompression dosn't know when to stop so you have remember how big the 
      uncompressed waveform is and cut it off.
      Maybe in the future this can be fixed in the deltacompression class but for now 
      the "user" ie I3DOMLaunch has to fix it.
      */
      new_waveform.resize(128);

      ENSURE(orig_waveform==new_waveform);
    }

}

