# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

#
# $Id$
#

i3_project(tableio
	DOCS_DIR resources/docs
	PYTHON_DIR python
)

set(TABLEIO_SOURCES
  private/tableio/*.cxx
  private/tableio/converter/*.cxx
  private/tableio/broadcaster/*.cxx
  private/tableio/textwriter/*.cxx
  private/tableio/utils/*.cxx
  private/tableio/detail/*.cxx
)

set(TABLEIO_TOOLS boost python)

set(TABLEIO_PYBINDINGS
  private/pybindings/I3TableRowDescription.cxx
  private/pybindings/I3TableRow.cxx
  private/pybindings/I3Converter.cxx
  private/pybindings/I3ConverterBundle.cxx
  private/pybindings/I3TableService.cxx
  private/pybindings/I3Table.cxx
  private/pybindings/I3TableWriter.cxx
  private/pybindings/I3TableTranscriber.cxx
  private/pybindings/I3BroadcastTableService.cxx
  private/pybindings/I3CSVTableService.cxx
  private/pybindings/I3Datatype.cxx
  private/pybindings/type_helpers.cxx
  private/pybindings/module.cxx
)

set(TABLEIO_TEST_EXECS
  private/test/main.cxx
  private/test/I3DOMLaunchConverterTest.cxx
  private/test/I3DatatypeTest.cxx
  private/test/I3DoubleConverterTest.cxx
  private/test/I3TableRowDescriptionTest.cxx
  private/test/I3TableRowTest.cxx
  private/test/I3VectorI3ParticleConverterTest.cxx
  private/test/InheritanceConversionTest.cxx
)

set (TABLEIO_TESTS
  resources/test/test_padding.py
  resources/test/test_pybindings.py
  resources/test/test_recopulsemask.py
  resources/test/test_recopulse_options.py
  resources/test/test_subevents.py
  resources/test/test_I3MCTree.py
  resources/test/test_inheritance_conversion.py
  resources/test/test_converters.py
)

if(ROOT_FOUND)
  list(APPEND TABLEIO_SOURCES
    private/tableio/rootwriter/I3ROOTBranchWrapper.cxx
    private/tableio/rootwriter/I3ROOTBranchWrapperData.cxx
    private/tableio/rootwriter/I3ROOTBranchWrapperEnum.cxx
    private/tableio/rootwriter/I3ROOTTable.cxx
    private/tableio/rootwriter/I3ROOTTableService.cxx
  )
  list(APPEND TABLEIO_TOOLS root)
  list(APPEND TABLEIO_PYBINDINGS
    private/pybindings/I3ROOTTableService.cxx
  )
  list(APPEND TABLEIO_TEST_EXECS
    private/tableio/rootwriter/test/I3ROOTBranchWrapperEnum.cxx
    private/tableio/rootwriter/test/I3ROOTRoundTripTest.cxx
    private/tableio/rootwriter/test/char_tests.cxx
  )
  list(APPEND TABLEIO_TESTS
    resources/test/rootwriter/test_example_scripts.py
    resources/test/rootwriter/test_rootfile_contents_pyroot.py
    resources/test/rootwriter/test_rootfile_contents_uproot.py
  )

else(ROOT_FOUND)
  colormsg(CYAN "+-- ROOT has been disabled in your configuration.")
  colormsg(CYAN "+-- Skipping build of rootwriter within ${PROJECT_NAME}")
endif(ROOT_FOUND)

IF(HDF5_FOUND)
  # add the path to the place where the hdf5 lib header files exist
  INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIR})
  ADD_DEFINITIONS(-DH5_USE_16_API)

  list(APPEND TABLEIO_SOURCES
    private/tableio/hdfwriter/I3HDFTable.cxx
    private/tableio/hdfwriter/I3HDFTableService.cxx
    private/tableio/hdfwriter/hdf5_opt.c
  )
  list(APPEND TABLEIO_TOOLS zlib hdf5)
  list(APPEND TABLEIO_PYBINDINGS
    private/pybindings/I3HDFTableService.cxx
  )
  list(APPEND TABLEIO_TEST_EXECS
    private/tableio/hdfwriter/test/I3HDFRoundTripTest.cxx
  )
  list(APPEND TABLEIO_TESTS
    resources/test/hdfwriter/test_double_open.py
    resources/test/hdfwriter/test_mix.sh
    resources/test/hdfwriter/test_slash_in_key_name.py
  )

  # Link manipulation scripts into $I3_BUILD/bin for ease of use.
  i3_executable_script(hdfwriter_merge resources/examples/hdfwriter_merge.py)
  i3_executable_script(hdfwriter_mix resources/examples/hdfwriter_mix.py)
  i3_executable_script(hdfwriter_book resources/examples/hdfwriter_book.py)

ELSE(HDF5_FOUND)

  colormsg(CYAN   "+-- hdf5 library not found, skipping build of hdfwriter within ${PROJECT_NAME}")
  colormsg(YELLOW "*** Ask your admin to install the hdf5 package.")

ENDIF(HDF5_FOUND)

IF(SQLITE3_FOUND)

  list(APPEND TABLEIO_SOURCES
    private/tableio/sqlitewriter/I3SQLiteTable.cxx
    private/tableio/sqlitewriter/I3SQLiteTableService.cxx
  )
  list(APPEND TABLEIO_TOOLS sqlite3)
  list(APPEND TABLEIO_PYBINDINGS
    private/pybindings/I3SQLiteTableService.cxx
  )
  list(APPEND TABLEIO_TESTS
    resources/test/sqlitewriter/sqlite_book_I3MCTree.py
  )

  # add executables
  i3_executable(sqlite_writer_merge_tables
    private/tableio/sqlitewriter/executables/merge_tables.cxx
    USE_TOOLS boost python sqlite3
    USE_PROJECTS icetray dataclasses tableio dataio serialization
  )
  i3_executable(sqlitewriter_delete_tables
    private/tableio/sqlitewriter/executables/delete_tables.cxx
    USE_TOOLS boost python sqlite3
    USE_PROJECTS icetray dataclasses tableio dataio serialization
  )
  i3_executable(sqlitewriter_concatenate
    private/tableio/sqlitewriter/executables/concatenate.cxx
    USE_TOOLS boost python sqlite3
    USE_PROJECTS icetray dataclasses tableio dataio serialization
  )

ELSE(SQLITE3_FOUND)

    colormsg(CYAN   "+-- sqlite3 library not found, skipping build of sqlitewriter within ${PROJECT_NAME}")
    colormsg(YELLOW "*** Ask your admin to install the sqlite3 package.")

ENDIF(SQLITE3_FOUND)

IF(ARROW_FOUND)

  list(APPEND TABLEIO_SOURCES
    private/tableio/parquetwriter/I3ParquetTable.cxx
    private/tableio/parquetwriter/I3ParquetTableService.cxx
  )
  list(APPEND TABLEIO_TOOLS arrow)
  list(APPEND TABLEIO_PYBINDINGS
    private/pybindings/I3ParquetTableService.cxx
  )
  list(APPEND TABLEIO_TESTS
    resources/test/parquetwriter/parquet_book_I3MCTree.py
  )

  # add executables
  i3_executable(parquetwriter_concatenate
    private/tableio/parquetwriter/executables/concatenate.cxx
    USE_TOOLS boost python arrow
    USE_PROJECTS icetray dataclasses tableio dataio serialization
  )
  i3_executable(parquetwriter_merge_fields
    private/tableio/parquetwriter/executables/merge_fields.cxx
    USE_TOOLS boost python arrow
    USE_PROJECTS icetray dataclasses tableio dataio serialization
  )

ELSE(ARROW_FOUND)

    colormsg(CYAN   "+-- apache arrow library not found, skipping build of parquetwriter within ${PROJECT_NAME}")
    colormsg(YELLOW "*** Ask your admin to install the apache arrow package.")

ENDIF(ARROW_FOUND)

i3_add_library(tableio
  ${TABLEIO_SOURCES}
  USE_TOOLS ${TABLEIO_TOOLS}
  USE_PROJECTS serialization icetray dataclasses dataio
)

i3_add_pybindings(tableio
  ${TABLEIO_PYBINDINGS}
  USE_TOOLS ${TABLEIO_TOOLS}
  USE_PROJECTS icetray tableio
)

i3_add_pybindings(tableio_dataclasses
  private/pybindings/dataclasses_converters.cxx
  USE_TOOLS boost python
  USE_PROJECTS icetray tableio dataclasses
)

i3_test_executable(test
    ${TABLEIO_TEST_EXECS}
    USE_TOOLS ${TABLEIO_TOOLS}
    USE_PROJECTS dataclasses icetray phys-services tableio
)

i3_test_scripts(
    ${TABLEIO_TESTS}
)

if(ROOT_FOUND)
  set_property(TEST tableio::test_rootfile_contents_pyroot.py PROPERTY SKIP_REGULAR_EXPRESSION "^Skipping test")
  set_property(TEST tableio::test_rootfile_contents_uproot.py PROPERTY SKIP_REGULAR_EXPRESSION "^Skipping test")
endif(ROOT_FOUND)

# abuse i3_project to add import compatibility shims for rootwriter
if(ROOT_FOUND)
  i3_project(rootwriter PYTHON_DIR shims/rootwriter)
endif(ROOT_FOUND)

# abuse i3_project to add import compatibility shims for hdfwriter
if(HDF5_FOUND)
  i3_project(hdfwriter PYTHON_DIR shims/hdfwriter)
endif(HDF5_FOUND)
