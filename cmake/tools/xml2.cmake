if (SYSTEM_PACKAGES)
  find_path(LIBXML2_ROOT_DIR
    NAMES include/libxml2/libxml/parser.h
  )
  TOOLDEF (xml2
    ${LIBXML2_ROOT_DIR}/include/libxml2
    libxml/parser.h
    "lib/libxml2 lib/x86_64-linux-gnu"
    NONE
    xml2)
endif (SYSTEM_PACKAGES)

if (NOT XML2_FOUND)
  TOOLDEF (xml2
    include/libxml2-2.7.8/libxml2
    libxml/parser.h
    lib/libxml2-2.7.8
    NONE
    xml2)
endif (NOT XML2_FOUND)
