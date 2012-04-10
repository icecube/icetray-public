#
#  $Id$
#  
#  Copyright (C) 2007
#  Troy D. Straszheim  <troy@icecube.umd.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#  
#  This file is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
#  
i3_project(dataio
  PYTHON_DIR python
  DOCS_DIR resources/docs
  )

i3_add_library (dataio 
  private/dataio-modules/*.cxx private/dataio/*.cxx
  USE_PROJECTS icetray dataclasses interfaces
  USE_TOOLS log4cplus boost python 
)
target_link_libraries(dataio ${BOOST_DATE_TIME})

i3_executable(shovel private/shovel/*.cxx
  USE_PROJECTS dataio
  USE_TOOLS cdk ncurses
  )

i3_executable(extract private/extract/*.cxx
  USE_PROJECTS dataio)

i3_executable(strip private/strip/*.cxx
  USE_PROJECTS dataio)

i3_executable(scan private/scan/*.cxx
  USE_PROJECTS dataio)

i3_test_executable(test private/test/*.cxx
  USE_PROJECTS dataio)

no_dotfile_glob(tmp_SCRIPTS resources/test/*.py)
if (NOT LIBARCHIVE_FOUND)
#  colormsg(CYAN "+-- libarchive *not* found, omitting test \"resources/test/o7_libarchive_read.py\"")
  LIST(REMOVE_ITEM tmp_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/test/o7_libarchive_read.py")
endif (NOT LIBARCHIVE_FOUND)
i3_test_scripts(${tmp_SCRIPTS})

add_subdirectory(private/pybindings)

# force dependencies
set_property(TEST dataio::a_nocompression.py APPEND PROPERTY DEPENDS dataio::a2_verify_daqtimes_change.py)
set_property(TEST dataio::b2_bz_default.py APPEND PROPERTY DEPENDS dataio::a_nocompression.py)
set_property(TEST dataio::b_default.py APPEND PROPERTY DEPENDS dataio::b2_bz_default.py)
set_property(TEST dataio::b_droporphanstreams.py APPEND PROPERTY DEPENDS dataio::b_default.py)
set_property(TEST dataio::c2_bz_best.py APPEND PROPERTY DEPENDS dataio::b_droporphanstreams.py)
set_property(TEST dataio::cant_iterate_on_writing_file.py APPEND PROPERTY DEPENDS dataio::c2_bz_best.py)
set_property(TEST dataio::c_best.py APPEND PROPERTY DEPENDS dataio::cant_iterate_on_writing_file.py)
set_property(TEST dataio::d2_bz_fastest.py APPEND PROPERTY DEPENDS dataio::c_best.py)
set_property(TEST dataio::d_fastest.py APPEND PROPERTY DEPENDS dataio::d2_bz_fastest.py)
set_property(TEST dataio::e_read_compress_data_from_Linux-i386.py APPEND PROPERTY DEPENDS dataio::d_fastest.py)
set_property(TEST dataio::frame_merging_test.py APPEND PROPERTY DEPENDS dataio::e_read_compress_data_from_Linux-i386.py)
set_property(TEST dataio::f_read_compressed_data_from_MacOSX-libstdc++6-ppc.py APPEND PROPERTY DEPENDS dataio::frame_merging_test.py)
set_property(TEST dataio::g_regex_filter_on_read_and_write.py APPEND PROPERTY DEPENDS dataio::f_read_compressed_data_from_MacOSX-libstdc++6-ppc.py)
set_property(TEST dataio::h2_verify_read_filter.py APPEND PROPERTY DEPENDS dataio::g_regex_filter_on_read_and_write.py)
set_property(TEST dataio::h_verify_write_filter.py APPEND PROPERTY DEPENDS dataio::h2_verify_read_filter.py)
set_property(TEST dataio::i_adds_mutineer.py APPEND PROPERTY DEPENDS dataio::h_verify_write_filter.py)
set_property(TEST dataio::j_fatals_reading_mutineer.py APPEND PROPERTY DEPENDS dataio::i_adds_mutineer.py)
set_property(TEST dataio::k1_passthru_mutineer.py APPEND PROPERTY DEPENDS dataio::j_fatals_reading_mutineer.py)
set_property(TEST dataio::k2a_get_finds_mutineer.py APPEND PROPERTY DEPENDS dataio::k1_passthru_mutineer.py)
set_property(TEST dataio::k2b_deleteunregistered_deletes_mutineer.py APPEND PROPERTY DEPENDS dataio::k2a_get_finds_mutineer.py)
set_property(TEST dataio::k2_verify_mutineer_passthru.py APPEND PROPERTY DEPENDS dataio::k2b_deleteunregistered_deletes_mutineer.py)
set_property(TEST dataio::k3_skipkeys_works.py APPEND PROPERTY DEPENDS dataio::k2_verify_mutineer_passthru.py)
set_property(TEST dataio::k4_skipkeys_works_2.py APPEND PROPERTY DEPENDS dataio::k3_skipkeys_works.py)
set_property(TEST dataio::l_read_list_of_files.py APPEND PROPERTY DEPENDS dataio::k4_skipkeys_works_2.py)
set_property(TEST dataio::make_alphabet_file.py APPEND PROPERTY DEPENDS dataio::l_read_list_of_files.py)
set_property(TEST dataio::m_multifile_1byte.py APPEND PROPERTY DEPENDS dataio::make_alphabet_file.py)
set_property(TEST dataio::multiple_gzipped_files_catted_together.py APPEND PROPERTY DEPENDS dataio::m_multifile_1byte.py)
set_property(TEST dataio::n2_read_list_of_files.py APPEND PROPERTY DEPENDS dataio::multiple_gzipped_files_catted_together.py)
set_property(TEST dataio::n_read_list_of_files.py APPEND PROPERTY DEPENDS dataio::n2_read_list_of_files.py)
set_property(TEST dataio::o0_multibyte_1000000.py APPEND PROPERTY DEPENDS dataio::n_read_list_of_files.py)
set_property(TEST dataio::o0_multibyte_metadatadup.py APPEND PROPERTY DEPENDS dataio::o0_multibyte_1000000.py)
set_property(TEST dataio::o1_verify_multibyte_frames.py APPEND PROPERTY DEPENDS dataio::o0_multibyte_1000000.py)
set_property(TEST dataio::o1b_verify_multibyte_frames.py APPEND PROPERTY DEPENDS dataio::o1_verify_multibyte_frames.py)
set_property(TEST dataio::o1_verify_multibyte_metadup_frames.py APPEND PROPERTY DEPENDS dataio::o1_verify_multibyte_frames.py)
set_property(TEST dataio::o2_split_gcd.py APPEND PROPERTY DEPENDS dataio::o1_verify_multibyte_metadup_frames.py)
set_property(TEST dataio::o3b_verify_multibyte_frames.py APPEND PROPERTY DEPENDS dataio::o2_split_gcd.py)
set_property(TEST dataio::o3_verify_multibyte_frames.py APPEND PROPERTY DEPENDS dataio::o3b_verify_multibyte_frames.py)
set_property(TEST dataio::o4_multiwriter_refuses_bogus_streamnames.py APPEND PROPERTY DEPENDS dataio::o3_verify_multibyte_frames.py)
set_property(TEST dataio::o5_write_1event.py APPEND PROPERTY DEPENDS dataio::o4_multiwriter_refuses_bogus_streamnames.py)
set_property(TEST dataio::o6_read_1event.py APPEND PROPERTY DEPENDS dataio::o5_write_1event.py)
set_property(TEST dataio::o7_libarchive_read.py APPEND PROPERTY DEPENDS dataio::o6_read_1event.py)
set_property(TEST dataio::pop_frame.py APPEND PROPERTY DEPENDS dataio::o7_libarchive_read.py)
set_property(TEST dataio::q_writenulls.py APPEND PROPERTY DEPENDS dataio::pop_frame.py)
set_property(TEST dataio::reader_can_read_alphabet_file.py APPEND PROPERTY DEPENDS dataio::q_writenulls.py)
set_property(TEST dataio::read_really_old_trayinfo_frame.py APPEND PROPERTY DEPENDS dataio::reader_can_read_alphabet_file.py)
set_property(TEST dataio::r_readnulls.py APPEND PROPERTY DEPENDS dataio::read_really_old_trayinfo_frame.py)
set_property(TEST dataio::s_i3reader_readnulls.py APPEND PROPERTY DEPENDS dataio::r_readnulls.py)
set_property(TEST dataio::test_randomaccess_file.py APPEND PROPERTY DEPENDS dataio::s_i3reader_readnulls.py)
set_property(TEST dataio::t_skip_infoframes.py APPEND PROPERTY DEPENDS dataio::test_randomaccess_file.py)
set_property(TEST dataio::writer_is_conditional.py APPEND PROPERTY DEPENDS dataio::t_skip_infoframes.py)
