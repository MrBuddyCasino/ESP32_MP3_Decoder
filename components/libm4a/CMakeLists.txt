idf_component_register(SRC_DIRS . 
                    INCLUDE_DIRS "include"
                    REQUIRES common)

target_compile_options(${COMPONENT_LIB} PRIVATE -DROCKBOX_LITTLE_ENDIAN)
