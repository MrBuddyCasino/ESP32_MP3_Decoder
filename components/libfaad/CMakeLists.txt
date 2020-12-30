idf_component_register( SRC_DIRS .
                    INCLUDE_DIRS "include" "codebook" .
                    )
target_compile_options(${COMPONENT_LIB} PRIVATE 
    -DHAVE_MEMCPY 
    -DSTDC_HEADERS 
    -DHAVE_INTTYPES_H 
    -DHAVE_INTTYPES_H 
    -DHAVE_STRINGS_H 
    -Wno-error=unused-function 
    -Wno-unused-function 
    -Wno-error=unused-variable 
    -Wno-unused-variable 
    -Wno-error=maybe-uninitialized 
    -Wno-maybe-uninitialized 
    -Wno-error=unused-value 
    -Wno-unused-but-set-variable)