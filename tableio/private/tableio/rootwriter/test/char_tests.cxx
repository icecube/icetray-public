/**
 * Copyright  (C) 2010 The Icecube Collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy$
 */


#include <I3Test.h>

#include <tableio/I3TableRow.h>
#include <tableio/I3TableRowDescription.h>
#include <tableio/rootwriter/I3ROOTTableService.h>
#include <tableio/rootwriter/I3ROOTTable.h>

#include <dataclasses/physics/I3EventHeader.h>

#include <TBranch.h>
#include <TFile.h>
#include <TLeaf.h>
#include <TTree.h>

#include <cstdio>

TEST_GROUP(char_tests);

TEST(signed_char_array) {
  const char *filename = "rootwriter_test_signed_char_array.root";
  const char *treename = "tTree";
  const char *fieldname = "char_array_field";
  const int nbytes = 5;
  const int8_t shift = -2;

  {
    // set up a writer service
    I3TableServicePtr writer_service = I3TableServicePtr(new I3ROOTTableService(filename));

    // build up a data type
    I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr(new I3TableRowDescription());
    desc->AddField<int8_t>(fieldname, "test-units", "An array of bytes (but not a string!)", nbytes);

    // create a table in the HDF file
    I3TablePtr table = writer_service->GetTable(treename, desc);
    I3TableRowPtr rows = table->CreateRow(1);

    I3EventHeaderConstPtr fake_header = I3EventHeaderConstPtr(new I3EventHeader());
    int8_t* char_array = rows->GetPointer<int8_t>(fieldname);
    for (int8_t i = 0; i < nbytes; ++i) char_array[i] = i-shift;
    table->AddRow(fake_header,rows);

    // clean up
    writer_service->Finish();
  }

  {
    // reopen the file for reading
    TFile file(filename);
    TTree *tree = dynamic_cast<TTree*>(file.Get(treename));

    ENSURE( tree != NULL, "The reader service gives us a non-null pointer" );
    ENSURE_EQUAL( tree->GetEntries(), 1, "The tree has exactly one entry (as written)" );

    // rootwriter should have converted the int8_t into Short_t
    TBranch *char_array_branch = tree->GetBranch(fieldname);
    ENSURE( char_array_branch != NULL, "The branch we just wrote should exist" );
    Short_t short_array[nbytes];
    char_array_branch->SetAddress(short_array);

    tree->GetEntry(0);

    for (Short_t i = 0; i < nbytes; ++i) {
      ENSURE_EQUAL(short_array[i], i-shift, "Values must match the ones we wrote.");
    }

    file.Close();
    std::remove(filename);
  }
}


TEST(single_signed_char) {
  const char *filename = "rootwriter_test_single_signed_char.root";
  const char *treename = "tTree";
  const char *fieldname = "char_field";
  const int8_t value = 52;

  {
    // set up a writer service
    I3TableServicePtr writer_service = I3TableServicePtr(new I3ROOTTableService(filename));

    // build up a data type
    I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr(new I3TableRowDescription());
    desc->AddField<int8_t>(fieldname, "test-units", "A single byte value");

    // create a table in the HDF file
    I3TablePtr table = writer_service->GetTable(treename, desc);
    I3TableRowPtr rows = table->CreateRow(1);

    I3EventHeaderConstPtr fake_header = I3EventHeaderConstPtr(new I3EventHeader());
    rows->Set<int8_t>(fieldname, value);
    table->AddRow(fake_header, rows);

    // clean up
    writer_service->Finish();
  }

  {
    // reopen the file for reading
    TFile file(filename);
    TTree *tree = dynamic_cast<TTree*>(file.Get(treename));

    ENSURE( tree != NULL, "The reader service gives us a non-null pointer" );
    ENSURE_EQUAL( tree->GetEntries(), 1, "The tree has exactly one entry (as written)" );

    // this should still be an integer
    TBranch *char_branch = tree->GetBranch(fieldname);
    ENSURE( char_branch != NULL, "The branch we just wrote should exist" );
    Char_t charVariable;
    char_branch->SetAddress(&charVariable);

    tree->GetEntry(0);

    ENSURE_EQUAL(charVariable, value, "Values match");

    file.Close();
    std::remove(filename);
  }
}


TEST(unsigned_char_array) {
  const char *filename = "rootwriter_test_unsigned_char_array.root";
  const char *treename = "tTree";
  const char *fieldname = "uchar_array_field";
  const int nbytes = 5;

  {
    // set up a writer service
    I3TableServicePtr writer_service = I3TableServicePtr(new I3ROOTTableService(filename));

    // build up a data type
    I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr(new I3TableRowDescription());
    desc->AddField<uint8_t>(fieldname, "test-units", "An array of unsigned bytes", nbytes);

    // create a table in the HDF file
    I3TablePtr table = writer_service->GetTable(treename, desc);
    I3TableRowPtr rows = table->CreateRow(1);

    I3EventHeaderConstPtr fake_header = I3EventHeaderConstPtr(new I3EventHeader());
    uint8_t* uchar_array = rows->GetPointer<uint8_t>(fieldname);
    for (uint8_t i = 0; i < nbytes; ++i) uchar_array[i] = i;
    table->AddRow(fake_header, rows);

    // clean up
    writer_service->Finish();
  }

  {
    // reopen the file for reading
    TFile file(filename);
    TTree *tree = dynamic_cast<TTree*>(file.Get(treename));

    ENSURE( tree != NULL, "The reader service gives us a non-null pointer" );
    ENSURE_EQUAL( tree->GetEntries(), 1, "The tree has exactly one entry (as written)" );

    // this branch should not be converted to 16bit length
    TBranch *uchar_array_branch = tree->GetBranch(fieldname);
    ENSURE( uchar_array_branch != NULL, "The branch we just wrote should exist" );
    UShort_t uchar_array[nbytes];
    uchar_array_branch->SetAddress(uchar_array);

    tree->GetEntry(0);

    for (UChar_t i = 0; i < nbytes; ++i) {
      ENSURE_EQUAL(uchar_array[i], i, "Values must match the ones we wrote.");
    }

    file.Close();
    std::remove(filename);
  }
}


TEST(single_unsigned_char) {
  const char *filename = "rootwriter_test_unsingle_signed_char.root";
  const char *treename = "tTree";
  const char *fieldname = "uchar_field";
  const uint8_t value = 129;

  {
    // set up a writer service
    I3TableServicePtr writer_service = I3TableServicePtr(new I3ROOTTableService(filename));

    // build up a data type
    I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr(new I3TableRowDescription());
    desc->AddField<uint8_t>(fieldname, "test-units", "A single byte value");

    // create a table in the HDF file
    I3TablePtr table = writer_service->GetTable(treename, desc);
    I3TableRowPtr rows = table->CreateRow(1);

    I3EventHeaderConstPtr fake_header = I3EventHeaderConstPtr(new I3EventHeader());
    rows->Set<uint8_t>(fieldname, value);
    table->AddRow(fake_header, rows);

    // clean up
    writer_service->Finish();
  }

  {
    // reopen the file for reading
    TFile file(filename);
    TTree *tree = dynamic_cast<TTree*>(file.Get(treename));

    ENSURE( tree != NULL, "The reader service gives us a non-null pointer" );
    ENSURE_EQUAL( tree->GetEntries(), 1, "The tree has exactly one entry (as written)" );

    // rootwriter should have converted the int8_t into Short_t
    TBranch *uchar_branch = tree->GetBranch(fieldname);
    ENSURE( uchar_branch != NULL, "The branch we just wrote should exist" );
    UChar_t ucharVariable;
    uchar_branch->SetAddress(&ucharVariable);

    tree->GetEntry(0);

    ENSURE_EQUAL(ucharVariable, value, "Values match");

    file.Close();
    std::remove(filename);
  }
}
