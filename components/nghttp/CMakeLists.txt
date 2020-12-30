idf_component_register(SRC_DIRS  "library" "port"
                    INCLUDE_DIRS "port/include" "include"
                     )

target_compile_options(${COMPONENT_LIB} PRIVATE -DHAVE_CONFIG_H )
