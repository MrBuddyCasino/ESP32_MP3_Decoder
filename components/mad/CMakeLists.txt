idf_component_register(SRC_DIRS .
                    INCLUDE_DIRS .)

target_compile_options(${COMPONENT_LIB} PRIVATE 
        -Wno-error=unused-label 
        -Wno-error=return-type 
        -Wno-error=missing-braces 
        -Wno-error=pointer-sign 
        -Wno-error=parentheses 
        -Wno-pointer-sign
        -Wno-implicit-fallthrough
        -Wno-return-type
        -Wno-unused-variable
)

