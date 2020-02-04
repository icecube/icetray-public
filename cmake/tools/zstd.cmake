
if (SYSTEM_PACKAGES)
  find_path(ZSTD_INCLUDE_DIR
    NAMES include/zstd.hpp)
  tooldef(zstd
    ${ZSTD_INCLUDE_DIR}
    zstd.h
    lib
    NONE
    zstd
    )
endif ()

