/**
 * Copyright  (C) 2010 The Icecube Collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy$
 */


#include <I3Test.h>
#include <limits.h>
#include <boost/filesystem.hpp>

#include <tableio/I3TableRow.h>
#include <tableio/I3TableRowDescription.h>
#include <tableio/rootwriter/I3ROOTTableService.h>
#include <tableio/rootwriter/I3ROOTTable.h>

#include <dataclasses/physics/I3EventHeader.h>

#include <TFile.h>
#include <TLeaf.h>
#include <TTree.h>

TEST_GROUP(I3ROOTRoundTripTests);

TEST(resurrection) {
  const char *filename = "I3ROOTRoundTripTest.root";
  const char *treename = "paso_double";
  const char *count_name = "Count_paso_double";

  const char *double_name = "double";
  const char *double_description = "A thing which is doubly precise, verily, the float is unworthy look upon.";
  const char *short_name = "shorty";
  const char *short_description = "Short is as short does. Unless things have to be aligned to 32 bits, that is.";
  const char *char_name = "charlatan";
  const char *char_description = "Speak your piece, but in 8 bits or less.";
  const char *long_name = "long_dooby_doo_long_long";
  const char *long_description = "The length of things which are long, measured in units of the height of one "
    "Oliver R. Smoot. Not entirely coincidentally, this was also the unit used to "
    "measure the length of the Harvard Bridge in October 1958 by rolling one "
    "Oliver R. Smoot end-over-end from one side of the Charles River to the other. "
    "Smoot chaired ANSI from 2001-2002 and was president of ISO from 2003-2004. "
    "[http://en.wikipedia.org/wiki/Smoot]";
  const char *bool_name = "farcicality";
  const char *bool_description = "Has this test become a farce?";
  const char *vector_name = "vector";
  const char *vector_description = "A vector of ints";
  const size_t vector_length = 256;

  // set up a writer service
  I3TableServicePtr writer_service = I3TableServicePtr(new I3ROOTTableService(filename));

  // build up a data type
  I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr(new I3TableRowDescription());
  desc->SetIsMultiRow(true); // we plan to write multiple rows
  desc->AddField<double>(double_name, "double-units", double_description);
  desc->AddField<int16_t>(short_name, "short-units", short_description);
  desc->AddField<int8_t>(char_name, "letter", char_description);
  desc->AddField<int64_t>(long_name, "smoots", long_description);

  bool thrown = false;
  try { desc->AddField<bool>(bool_name, "truth-units", bool_description); }
  catch (...) { thrown = true; }
  ENSURE( thrown, "Setting a boolean field with a non-empty unit string that is not 'bool' throws an error.");

  desc->AddField<bool>(bool_name, "", bool_description);

  desc->AddField<int32_t>(vector_name, "", vector_description, vector_length);

  // create a table in the HDF file
  I3TablePtr table = writer_service->GetTable(treename,desc);

  // get container, fill container, write container
  size_t nrows = 3;
  I3TableRowPtr rows = table->CreateRow(nrows);

  double doubleval = 3.14;
  int16_t shortval = 512;
  int8_t charval = std::numeric_limits<int8_t>::max();
  int64_t longval = std::numeric_limits<int64_t>::max();
  bool boolval = true;
  size_t i, j;

  for (i=0; i<nrows; i++) {
    rows->SetCurrentRow(i);
    rows->Set<double>(double_name, doubleval);
    rows->Set<int16_t>(short_name, shortval);
    rows->Set<int8_t>(char_name, charval);
    rows->Set<int64_t>(long_name, longval);
    rows->Set<bool>(bool_name, boolval);
    int32_t* vec = rows->GetPointer<int32_t>(vector_name);
    for (j=0; j < vector_length; j++) vec[j] = j;
  }

  I3EventHeaderConstPtr fake_header = I3EventHeaderConstPtr(new I3EventHeader());
  table->AddRow(fake_header,rows);

  // clean up
  writer_service->Finish();

  // reopen the file for reading
  TFile file(filename);
  TTree *tree = dynamic_cast<TTree*>(file.Get(treename));

  ENSURE( tree != NULL, "The reader service gives us a non-null pointer" );
  ENSURE_EQUAL( tree->GetEntries(), 1, "The tree has exactly one entry (as written)" );

  TBranch *countBranch = tree->GetBranch(count_name);
  ENSURE( countBranch != NULL, "Counter branch should exist" );
  uint64_t countVariable;
  countBranch->SetAddress(&countVariable);

  tree->GetEntry(0);

  TBranch *doubleBranch = tree->GetBranch(double_name);
  ENSURE( doubleBranch != NULL, "Branch was written but cannot be retrieved from the tree" );
  ENSURE_EQUAL( doubleBranch->GetLeaf(double_name)->GetLen(), int(countVariable), "Leaf has correct length" );
  double *doubleVariable = new double[countVariable];
  doubleBranch->SetAddress(doubleVariable);

  TBranch *shortBranch = tree->GetBranch(short_name);
  ENSURE( shortBranch != NULL, "Branch was written but cannot be retrieved from the tree" );
  ENSURE_EQUAL( shortBranch->GetLeaf(short_name)->GetLen(), int(countVariable), "Leaf has correct length" );
  int16_t *shortVariable = new short[countVariable];
  shortBranch->SetAddress(shortVariable);

  TBranch *charBranch = tree->GetBranch(char_name);
  ENSURE( charBranch != NULL, "Branch was written but cannot be retrieved from the tree" );
  ENSURE_EQUAL( charBranch->GetLeaf(char_name)->GetLen(), int(countVariable), "Leaf has correct length" );
  int16_t *charVariable = new int16_t[countVariable];
  charBranch->SetAddress(charVariable);

  TBranch *longBranch = tree->GetBranch(long_name);
  ENSURE( longBranch != NULL, "Branch was written but cannot be retrieved from the tree" );
  ENSURE_EQUAL( longBranch->GetLeaf(long_name)->GetLen(), int(countVariable), "Leaf has correct length" );
  int64_t *longVariable = new int64_t[countVariable];
  longBranch->SetAddress(longVariable);

  TBranch *boolBranch = tree->GetBranch(bool_name);
  ENSURE( boolBranch != NULL, "Branch was written but cannot be retrieved from the tree" );
  ENSURE_EQUAL( boolBranch->GetLeaf(bool_name)->GetLen(), int(countVariable), "Leaf has correct length" );
  bool *boolVariable = new bool[countVariable];
  boolBranch->SetAddress(boolVariable);

  TBranch *vectorBranch = tree->GetBranch(vector_name);
  ENSURE( vectorBranch != NULL, "Branch was written but cannot be retrieved from the tree" );
  ENSURE_EQUAL( vectorBranch->GetLeaf(vector_name)->GetLen(), int(vector_length*countVariable),
		"Vector has correct length" );
  int32_t vectorVariable[vector_length*countVariable];
  vectorBranch->SetAddress(vectorVariable);

  tree->GetEntry(0);

  for (i = 0; i < countVariable; ++i) {
    ENSURE_EQUAL(doubleVariable[i], doubleval, "Double values match");
    ENSURE_EQUAL(shortVariable[i], shortval, "Short values match");
    ENSURE_EQUAL(charVariable[i], charval, "Char values match");
    ENSURE_EQUAL(longVariable[i], longval, "Long values match");
    ENSURE_EQUAL(boolVariable[i], boolval, "Bool values match");

    for (j = 0; j < vector_length; ++j)
      ENSURE_EQUAL(vectorVariable[i*vector_length + j], int32_t(j), "Vector values match");
  }

  file.Close();

  boost::filesystem::remove(filename);
}
