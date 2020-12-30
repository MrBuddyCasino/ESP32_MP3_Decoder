idf_component_register(SRCS 
                            "http.c"
                    INCLUDE_DIRS "include"
                    REQUIRES nvs_flash nghttp url_parser)


target_compile_options(${COMPONENT_LIB} PRIVATE 
        -Wno-error=unused-label 
        -Wno-error=return-type 
        -Wno-error=missing-braces 
        -Wno-error=pointer-sign 
        -Wno-error=parentheses
)