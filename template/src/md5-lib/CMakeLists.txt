add_library(md5-lib STATIC)

target_sources(
    md5-lib
    PRIVATE
        "md5.h"
        "md5.cpp"
)