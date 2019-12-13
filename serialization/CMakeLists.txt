i3_project(serialization
  DOCS_DIR resources/docs)

i3_add_library (serialization 
  private/archive/portable_binary_archive.cpp
  private/serialization/archive_exception.cpp
  private/serialization/basic_archive.cpp
  private/serialization/basic_iarchive.cpp
  private/serialization/basic_iserializer.cpp
  private/serialization/basic_oarchive.cpp
  private/serialization/basic_oserializer.cpp
  private/serialization/basic_pointer_iserializer.cpp
  private/serialization/basic_pointer_oserializer.cpp
  private/serialization/basic_serializer_map.cpp
  private/serialization/basic_text_iprimitive.cpp
  private/serialization/basic_text_oprimitive.cpp
  private/serialization/basic_xml_archive.cpp
  private/serialization/binary_iarchive.cpp
  private/serialization/binary_oarchive.cpp
  private/serialization/codecvt_null.cpp
  private/serialization/extended_type_info_no_rtti.cpp
  private/serialization/extended_type_info_typeid.cpp
  private/serialization/extended_type_info.cpp
  private/serialization/polymorphic_iarchive.cpp
  private/serialization/polymorphic_oarchive.cpp
  private/serialization/stl_port.cpp
  private/serialization/text_iarchive.cpp
  private/serialization/text_oarchive.cpp
#  private/serialization/text_wiarchive.cpp
#  private/serialization/text_woarchive.cpp
#  private/serialization/utf8_codecvt_facet.cpp
  private/serialization/void_cast.cpp
  private/serialization/xml_archive_exception.cpp
  private/serialization/xml_grammar.cpp
  private/serialization/xml_iarchive.cpp
  private/serialization/xml_oarchive.cpp
  USE_TOOLS boost
)

i3_test_executable(test
  private/test/main.cxx
  private/test/A.cpp
  private/test/test_array.cpp
  private/test/test_binary.cpp
  private/test/test_bitset.cpp
  private/test/test_codecvt_null.cpp
  private/test/test_collection_size_type.cpp
  private/test/test_complex.cpp
  private/test/test_contained_class.cpp
  private/test/test_cyclic_ptrs.cpp
  private/test/test_delete_pointer.cpp
  private/test/test_deque.cpp
  private/test/test_derived_class_ptr.cpp
  private/test/test_derived_class.cpp
  private/test/test_derived.cpp
  private/test/test_diamond_complex.cpp
  private/test/test_diamond.cpp
  #private/test/test_enable_shared_from_this.cpp # doesn't currently work
  private/test/test_helper_support.cpp
  private/test/test_inclusion.cpp
  private/test/test_iterators_base64.cpp
  private/test/test_iterators.cpp
  private/test/test_list_ptrs.cpp
  private/test/test_list.cpp
  private/test/test_map.cpp
  private/test/test_mi.cpp
  private/test/test_mult_archive_types.cpp
  private/test/test_multiple_inheritance.cpp
  private/test/test_multiple_ptrs.cpp
  private/test/test_new_operator.cpp
  private/test/test_no_rtti.cpp
  private/test/test_non_default_ctor.cpp
  private/test/test_non_default_ctor2.cpp
  private/test/test_non_intrusive.cpp
  private/test/test_null_ptr.cpp
  private/test/test_nvp.cpp
  private/test/test_object.cpp
  private/test/test_optional.cpp
  #private/test/test_pimpl.cpp # this test seems to have been half-baked?
  private/test/polymorphic_base.cpp
  private/test/polymorphic_derived1.cpp
  private/test/polymorphic_derived2.cpp
  private/test/test_polymorphic_A.cpp
  private/test/test_polymorphic.cpp
  private/test/test_polymorphic2.cpp
  private/test/test_polymorphic2imp.cpp
  private/test/test_primitive.cpp
  private/test/test_priority_queue.cpp
  private/test/test_private_base.cpp
  private/test/test_private_ctor.cpp
  private/test/test_queue.cpp
  private/test/test_recursion.cpp
  private/test/test_registered.cpp
  private/test/test_reset_object_address.cpp
  private/test/test_set.cpp
  private/test/test_shared_ptr_132.cpp
  private/test/test_simple_class_ptr.cpp
  private/test/test_simple_class.cpp
  private/test/test_singleton.cpp
  private/test/test_smart_cast.cpp
  private/test/test_split.cpp
  private/test/test_stack.cpp
  #private/test/test_string_4g.cpp # need >4GB memory
  private/test/test_tracking.cpp
  private/test/test_unique_ptr.cpp
  #private/test/test_utf8_codecvt.cpp # broken on linux
  private/test/test_valarray.cpp
  private/test/test_variant.cpp
  private/test/test_vector.cpp
  private/test/test_version_type.cpp
  private/test/test_void_cast.cpp

  USE_TOOLS boost python
  USE_PROJECTS serialization
)

i3_test_executable(test_class_info_save
  private/test/main.cxx
  private/test/test_class_info_save.cpp
  USE_TOOLS boost python
  USE_PROJECTS serialization
)

i3_test_executable(test_class_info_load
  private/test/main.cxx
  private/test/test_class_info_load.cpp
  USE_TOOLS boost python
  USE_PROJECTS serialization
)

set_property(TEST serialization::test_class_info_load APPEND PROPERTY DEPENDS serialization::test_class_info_save)

i3_test_executable(test_exported
  private/test/main.cxx
  private/test/polymorphic_base.cpp
  private/test/test_exported.cpp
  USE_TOOLS boost python
  USE_PROJECTS serialization
)

i3_test_executable(test_shared_ptr
  private/test/main.cxx
  private/test/test_shared_ptr.cpp
  USE_TOOLS boost python
  USE_PROJECTS serialization
)

i3_test_executable(test_shared_ptr_multi_base
  private/test/main.cxx
  private/test/test_shared_ptr_multi_base.cpp
  USE_TOOLS boost python
  USE_PROJECTS serialization
)

# This fails on Ubuntu, both 16.04 and 14.04
#add_test(NAME serialization::test_compile_only
#  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
#  COMMAND ${CMAKE_BINARY_DIR}/serialization/resources/test/compile-only-tests.sh
#)
#
#SET_TESTS_PROPERTIES(serialization::test_compile_only
#  PROPERTIES ENVIRONMENT "CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")

#probably need own executables?
# test_dll_exported.cpp
# test_dll_plugin.cpp
# test_dll_simple.cpp
