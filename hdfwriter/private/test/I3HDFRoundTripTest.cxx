/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */



#include <I3Test.h>
#include <limits.h>
#include <boost/filesystem.hpp>

#include <tableio//I3TableRow.h>
#include <tableio/I3TableRowDescription.h>
#include <hdfwriter/I3HDFTableService.h>
#include <hdfwriter/I3HDFTable.h>

#include <dataclasses/physics/I3EventHeader.h>

TEST_GROUP(I3HDFRoundTripTests);

TEST(resurrection) {
   // set up a writer service
  const std::string filename = "I3HDFRoundTripTest.hd5";
   I3TableServicePtr writer_service = I3TableServicePtr(new I3HDFTableService(filename));

   // build up a data type
   I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr(new I3TableRowDescription());
   desc->SetIsMultiRow(true); // we plan to write multiple rows
   desc->AddField<double>("double","double-units","A thing which is doubly precise, verily, the float is unworthy look upon.");
   desc->AddField<short>("shorty","short-units","Short is as short does. Unless things have to be aligned to 32 bits, that is.");
   desc->AddField<char>("charlatan","letter","Speak your piece, but in 8 bits or less.");
   desc->AddField<long>("long_dooby_doo_long_long","smoots",
         "The length of things which are long, measured in units of the height of one \
         Oliver R. Smoot. Not entirely coincidentally, this was also the unit used to \
         measure the length of the Harvard Bridge in October 1958 by rolling one \
         Oliver R. Smoot end-over-end from one side of the Charles River to the other.\
         Smoot chaired ANSI from 2001-2002 and was president of ISO from 2003-2004.\
         [http://en.wikipedia.org/wiki/Smoot]");
   
	bool thrown = false;
	try { desc->AddField<bool>("farcicality","truth-units","Has this test become a farce?"); }
	catch (...) { thrown = true; }
	ENSURE( thrown, "Setting a boolean field with a non-empty unit string that is not 'bool' throws an error.");
   
	desc->AddField<bool>("farcicality","","Has this test become a farce?");
	
    desc->AddField<int>("vector","","A vector of ints",256);
   
   
   // create a table in the HDF file
   I3TablePtr table = writer_service->GetTable("paso_double",desc);
   
   // get container, fill container, write container
   size_t nrows = 3;
   I3TableRowPtr rows = table->CreateRow(nrows);

   double doubleval = 3.14;
   short shortval = 512;
   short charval = CHAR_MAX;
   long longval = LONG_MAX;
   bool boolval = true;
   size_t i,j;
   
   for(i=0; i<nrows; i++) {
      rows->SetCurrentRow(i);
      rows->Set<double>("double",doubleval);
      rows->Set<short>("shorty",shortval);
      rows->Set<char>("charlatan",charval);
      rows->Set<long>("long_dooby_doo_long_long",longval);
      rows->Set<bool>("farcicality",boolval);
      int* vec = rows->GetPointer<int>("vector");
      for (j=0;j<256;j++) vec[j] = j;
   }

   I3EventHeaderConstPtr fake_header = I3EventHeaderConstPtr(new I3EventHeader());
   table->AddRow(fake_header,rows);
   
   // clean up
   writer_service->Finish();
   
   // reopen the file for reading
   I3TableServicePtr reader_service = I3TableServicePtr(new I3HDFTableService(filename,1,'r'));
   
   // convention: desc = NULL => only return table if it exists on disk
   I3TablePtr zombie_table = reader_service->GetTable("paso_double",I3TableRowDescriptionPtr());
   
   ENSURE( zombie_table != NULL, "The reader service gives us a non-null pointer");
   
   I3TableRowDescriptionConstPtr zombie_desc = zombie_table->GetDescription();
   
   // compare descriptions
   std::vector<std::string>::const_iterator os_it, zs_it;
   for (os_it  = desc->GetFieldNames().begin(), zs_it = zombie_desc->GetFieldNames().begin(), i=0;
        os_it != desc->GetFieldNames().end();
        os_it++, zs_it++, i++) {
            ENSURE_EQUAL( *os_it, *zs_it, "Field names match");
   }
   for (os_it  = desc->GetFieldUnits().begin(), zs_it = zombie_desc->GetFieldUnits().begin(), i=0;
        os_it != desc->GetFieldUnits().end();
        os_it++, zs_it++, i++) {
            ENSURE_EQUAL( *os_it, *zs_it, "Field units match");
   }
   for (os_it  = desc->GetFieldDocStrings().begin(), zs_it = zombie_desc->GetFieldDocStrings().begin(), i=0;
        os_it != desc->GetFieldDocStrings().end();
        os_it++, zs_it++, i++) {
            ENSURE_EQUAL( *os_it, *zs_it, "Field doc strings match");
   }
   
   std::vector<size_t>::const_iterator osize_it,zsize_it;
   for (osize_it  = desc->GetFieldTypeSizes().begin(), zsize_it = zombie_desc->GetFieldTypeSizes().begin(), i=0;
        osize_it != desc->GetFieldTypeSizes().end();
        osize_it++, zsize_it++, i++) {
            ENSURE_EQUAL( *osize_it, *zsize_it, "Field type sizes match.");
   }
   for (osize_it  = desc->GetFieldByteOffsets().begin(), zsize_it = zombie_desc->GetFieldByteOffsets().begin(), i=0;
        osize_it != desc->GetFieldByteOffsets().end();
        osize_it++, zsize_it++, i++) {
            ENSURE_EQUAL( *osize_it, *zsize_it, "Field byte offsets match");
   }
   for (osize_it  = desc->GetFieldArrayLengths().begin(), zsize_it = zombie_desc->GetFieldArrayLengths().begin(), i=0;
        osize_it != desc->GetFieldArrayLengths().end();
        osize_it++, zsize_it++, i++) {
            ENSURE_EQUAL( *osize_it, *zsize_it, "Field array offsets match");
   }
   
   std::vector<I3Datatype>::const_iterator ohid_it,zhid_it;
   for (ohid_it  = desc->GetFieldTypes().begin(), zhid_it = zombie_desc->GetFieldTypes().begin(), i=0;
        ohid_it != desc->GetFieldTypes().end();
        ohid_it++, zhid_it++, i++) {
         
         ENSURE_EQUAL( ohid_it->size, zhid_it->size, "Field I3Datatypes match");
         ENSURE_EQUAL( (int)ohid_it->kind, (int)zhid_it->kind, "Field I3Datatypes match");
         ENSURE_EQUAL( ohid_it->is_signed, zhid_it->is_signed, "Field I3Datatypes match");
         
         
   }
   
   // ReadRows is public only for I3HDFTable
   I3TableRowConstPtr zombie_rows = ((I3HDFTable*)(zombie_table.get()))->ReadRows(0,nrows);
   
   I3TableRowPtr castaway = boost::const_pointer_cast<I3TableRow>(zombie_rows);
   
   for (i=0; i<nrows; i++) {
      castaway->SetCurrentRow(i);
      ENSURE_EQUAL( doubleval, castaway->Get<double>("double"), "Read double is equal to set double."); 
      ENSURE_EQUAL( longval, castaway->Get<long>("long_dooby_doo_long_long"), "Read long is equal to set long."); 
      ENSURE_EQUAL( shortval, castaway->Get<short>("shorty"), "Read short is equal to set short.");
      ENSURE_EQUAL( charval, castaway->Get<char>("charlatan"), "Read char is equal to set char.");
      ENSURE_EQUAL( boolval, castaway->Get<bool>("farcicality"), "Read bool is equal to set bool.");
      int* vec = castaway->GetPointer<int>("vector");
      for (j=0;j<256;j++) ENSURE_EQUAL( (int)j, vec[j], "Vector contents are the same");
   }

   size_t obuffer_size = desc->GetTotalByteSize();
   size_t zbuffer_size = zombie_desc->GetTotalByteSize();
   
   ENSURE( obuffer_size > 0, "There's actually something in the write buffer");
   
   ENSURE_EQUAL( obuffer_size, zbuffer_size, "Written and read rows have equal-sized buffers.");
   ENSURE( memcmp(rows->GetPointer(),zombie_rows->GetPointer(),obuffer_size*nrows) == 0, "Written and read buffers are equal, byte for byte.");

   reader_service->Finish();

   boost::filesystem::remove(filename);
}
