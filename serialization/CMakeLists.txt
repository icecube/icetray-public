i3_project(serialization)

i3_add_library (serialization 
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

# TODO: get test up and running
#i3_test_executable(test private/test/*.cxx
#  USE_PROJECTS dataio)
