#
#  $Id$
#  
#  Copyright (C) 2007
#  Troy D. Straszheim  <troy@icecube.umd.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#  
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
#  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
#  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#  SUCH DAMAGE.
#  
#  SPDX-License-Identifier: BSD-2-Clause
#  
#  
i3_project(dataio
  PYTHON_DIR python
  DOCS_DIR resources/docs
  )

i3_add_library(dataio 
  private/dataio-modules/*.cxx private/dataio/*.cxx
  USE_PROJECTS serialization icetray dataclasses 
  USE_TOOLS boost python 
)
target_link_libraries(dataio ${BOOST_DATE_TIME})

i3_add_pybindings(dataio
  private/pybindings/I3File.cxx
  private/pybindings/I3FrameSequence.cxx
  private/pybindings/I3FileStager.cxx
  private/pybindings/module.cxx
  USE_PROJECTS icetray dataio
  USE_TOOLS boost python
  )

if(CDK_FOUND AND CURSES_FOUND)
  i3_executable(shovel private/shovel/*.cxx
    USE_PROJECTS dataio
    USE_TOOLS cdk curses
    )  
else()
  colormsg(CYAN "+-- dataio-shovel *not* included (missing CDK or ncurses libraries)")
endif()
  
# symlink for dataio-pyshovel
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink dataio-shovel dataio-pyshovel
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## expect is needed for the datio-shovel-test
find_program(EXPECT expect)

i3_test_executable(test private/test/*.cxx
  USE_PROJECTS dataio)

no_dotfile_glob(tmp_SCRIPTS resources/test/*.py)
if (NOT LIBARCHIVE_FOUND)
#  colormsg(CYAN "+-- libarchive *not* found, omitting test \"resources/test/o7_libarchive_read.py\"")
  LIST(REMOVE_ITEM tmp_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/test/o7_libarchive_read.py")
endif (NOT LIBARCHIVE_FOUND)

if (EXPECT)
  LIST(APPEND tmp_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/test/dataio-shovel-test")
endif ()

## something's busted w/ lucid's python+ssl
execute_process(COMMAND lsb_release -sc OUTPUT_VARIABLE LUCID ERROR_QUIET)
if (LUCID MATCHES "lucid")
  list(REMOVE_ITEM tmp_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/test/test_filestager.py")
endif ()
i3_test_scripts(${tmp_SCRIPTS})

# Many dependencies between tests.  Make them explicit.
# These tests use filtered.i3, created by dataio::g_regex_filter_on_read_and_write.py
set_property(TEST dataio::h2_verify_read_filter.py APPEND PROPERTY DEPENDS dataio::g_regex_filter_on_read_and_write.py)
set_property(TEST dataio::h_verify_write_filter.py APPEND PROPERTY DEPENDS dataio::g_regex_filter_on_read_and_write.py)

# These tests use pass1.i3, created by dataio::a_nocompression.py
set_property(TEST dataio::b2_bz_default.py APPEND PROPERTY DEPENDS dataio::a_nocompression.py)
set_property(TEST dataio::b_default.py APPEND PROPERTY DEPENDS dataio::a_nocompression.py)
set_property(TEST dataio::b_droporphanstreams.py APPEND PROPERTY DEPENDS dataio::a_nocompression.py)
set_property(TEST dataio::g_regex_filter_on_read_and_write.py APPEND PROPERTY DEPENDS dataio::a_nocompression.py)
set_property(TEST dataio::i_adds_mutineer.py APPEND PROPERTY DEPENDS dataio::a_nocompression.py)
set_property(TEST dataio::m_multifile_1byte.py APPEND PROPERTY DEPENDS dataio::a_nocompression.py)
set_property(TEST dataio::o0_multibyte_1000000.py APPEND PROPERTY DEPENDS dataio::a_nocompression.py)
set_property(TEST dataio::o0_multibyte_metadatadup.py APPEND PROPERTY DEPENDS dataio::a_nocompression.py)
set_property(TEST dataio::o2_split_gcd.py APPEND PROPERTY DEPENDS dataio::a_nocompression.py)
set_property(TEST dataio::o5_write_1event.py APPEND PROPERTY DEPENDS dataio::a_nocompression.py)
set_property(TEST dataio::q_writenulls.py APPEND PROPERTY DEPENDS dataio::a_nocompression.py)
set_property(TEST dataio::zst_test.py APPEND PROPERTY DEPENDS dataio::a_nocompression.py)

# These tests use withnulls.i3, created by dataio::q_writenulls.py
set_property(TEST dataio::r_readnulls.py APPEND PROPERTY DEPENDS dataio::q_writenulls.py)
set_property(TEST dataio::s_i3reader_readnulls.py APPEND PROPERTY DEPENDS dataio::q_writenulls.py)
set_property(TEST dataio::t_skip_infoframes.py APPEND PROPERTY DEPENDS dataio::q_writenulls.py)

# These tests use hasmutineer.i3.gz, created by dataio::i_adds_mutineer.py
set_property(TEST dataio::j_fatals_reading_mutineer.py APPEND PROPERTY DEPENDS dataio::i_adds_mutineer.py)
set_property(TEST dataio::k1_passthru_mutineer.py APPEND PROPERTY DEPENDS dataio::i_adds_mutineer.py)
set_property(TEST dataio::k2_verify_mutineer_passthru.py APPEND PROPERTY DEPENDS dataio::i_adds_mutineer.py)
set_property(TEST dataio::k2a_get_finds_mutineer.py APPEND PROPERTY DEPENDS dataio::i_adds_mutineer.py)
set_property(TEST dataio::k2b_deleteunregistered_deletes_mutineer.py APPEND PROPERTY DEPENDS dataio::i_adds_mutineer.py)

# This tests uses testm****.i3.gz, created by m_multifile_1byte.py
set_property(TEST dataio::n_read_list_of_files.py APPEND PROPERTY DEPENDS dataio::m_multifile_1byte.py)

# These tests use testmulti.****.i3, created by o0_multibyte_1000000.py
set_property(TEST dataio::o1_verify_multibyte_frames.py APPEND PROPERTY DEPENDS dataio::o0_multibyte_1000000.py)
set_property(TEST dataio::o1b_verify_multibyte_frames.py APPEND PROPERTY DEPENDS dataio::o0_multibyte_1000000.py)

# These tests use split*.i3, generated by o2_split_gcd.py
set_property(TEST dataio::o4_multiwriter_refuses_bogus_streamnames.py APPEND PROPERTY DEPENDS dataio::o2_split_gcd.py)
set_property(TEST dataio::o3b_verify_multibyte_frames.py APPEND PROPERTY DEPENDS dataio::o2_split_gcd.py)
set_property(TEST dataio::o3_verify_multibyte_frames.py APPEND PROPERTY DEPENDS dataio::o2_split_gcd.py)

# Some single dependencies
set_property(TEST dataio::o6_read_1event.py APPEND PROPERTY DEPENDS dataio::o5_write_1event.py)
set_property(TEST dataio::o4_multiwriter_refuses_bogus_streamnames.py APPEND PROPERTY DEPENDS dataio::o2_split_gcd.py)
set_property(TEST dataio::c2_bz_best.py APPEND PROPERTY DEPENDS dataio::b2_bz_default.py)
set_property(TEST dataio::c_best.py APPEND PROPERTY DEPENDS dataio::b_default.py)
set_property(TEST dataio::d2_bz_fastest.py APPEND PROPERTY DEPENDS dataio::c2_bz_best.py)
set_property(TEST dataio::d_fastest.py APPEND PROPERTY DEPENDS dataio::c_best.py)
set_property(TEST dataio::e_read_compress_data_from_Linux-i386.py APPEND PROPERTY DEPENDS dataio::d_fastest.py)
set_property(TEST dataio::o1_verify_multibyte_metadup_frames.py APPEND PROPERTY DEPENDS dataio::o0_multibyte_metadatadup.py)
set_property(TEST dataio::k2_verify_mutineer_passthru.py APPEND PROPERTY DEPENDS dataio::k1_passthru_mutineer.py)


# Explicitly make the cleanup depend on everything.  Let cmake figure out the dependencies
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::a2_verify_daqtimes_change.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::a_nocompression.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::b2_bz_default.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::b_default.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::b_droporphanstreams.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::c2_bz_best.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::cant_iterate_on_writing_file.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::c_best.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::d2_bz_fastest.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::d_fastest.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::e_read_compress_data_from_Linux-i386.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::frame_merging_test.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::f_read_compressed_data_from_MacOSX-libstdc++6-ppc.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::g_regex_filter_on_read_and_write.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::h2_verify_read_filter.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::h_verify_write_filter.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::i_adds_mutineer.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::j_fatals_reading_mutineer.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::k1_passthru_mutineer.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::k2a_get_finds_mutineer.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::k2b_deleteunregistered_deletes_mutineer.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::k2_verify_mutineer_passthru.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::k3_skipkeys_works.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::k4_skipkeys_works_2.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::l_read_list_of_files.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::make_alphabet_file.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::m_multifile_1byte.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::multiple_gzipped_files_catted_together.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::n_read_list_of_files.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::o0_multibyte_1000000.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::o0_multibyte_metadatadup.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::o1_verify_multibyte_frames.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::o1b_verify_multibyte_frames.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::o1_verify_multibyte_metadup_frames.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::o2_split_gcd.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::o3b_verify_multibyte_frames.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::o3_verify_multibyte_frames.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::o4_multiwriter_refuses_bogus_streamnames.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::o5_write_1event.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::o6_read_1event.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::pop_frame.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::q_writenulls.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::reader_can_read_alphabet_file.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::read_really_old_trayinfo_frame.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::r_readnulls.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::s_i3reader_readnulls.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::t_skip_infoframes.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::writer_is_conditional.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::zst_test.py)

# dataio-shovel-tests requires pass1.i3, created by dataio::a_nocompression.py
if (EXPECT)
set_property(TEST dataio::dataio-shovel-test APPEND PROPERTY DEPENDS dataio::a_nocompression.py)
set_property(TEST dataio::zz_cleanup.py APPEND PROPERTY DEPENDS dataio::dataio-shovel-test)
endif ()

if (LIBARCHIVE_FOUND)
# Move this test to the end.  It occasionally fails when run concurrently with
# other tests for some unknown reason.  It does not seem to share common files.
set_property(TEST dataio::o7_libarchive_read.py APPEND PROPERTY DEPENDS dataio::zz_cleanup.py)
set_property(TEST dataio::o7_libarchive_read.py PROPERTY SKIP_REGULAR_EXPRESSION "skipping archive-reading tests")
endif (LIBARCHIVE_FOUND)


# The following is used to test features
# related to dealing with unregistered classes.
# So it gets its own separate library.
i3_add_pybindings(test_unregistered
  private/test_unregistered/unregistered_track.cxx
  USE_PROJECTS icetray dataclasses
  USE_TOOLS boost python
  NOUNDERSCORE
  )
