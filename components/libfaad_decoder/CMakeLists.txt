idf_component_register(SRCS 
                            "libfaad_decoder.c"
                    INCLUDE_DIRS "include"
                    REQUIRES common libfaad libm4a audio_renderer )

target_compile_options(${COMPONENT_LIB} PRIVATE 
        -DHAVE_MEMCPY 
        -DSTDC_HEADERS 
        -DHAVE_INTTYPES_H 
        -DHAVE_INTTYPES_H 
        -DHAVE_STRINGS_H 
        -Wno-unused-variable 
        -Wno-unused-but-set-variable)

