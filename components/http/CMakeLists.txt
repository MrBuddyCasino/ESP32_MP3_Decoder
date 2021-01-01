idf_component_register(SRCS 
                            "http.c"
                    INCLUDE_DIRS "include"
                    REQUIRES nghttp
                    PRIV_REQUIRES nvs_flash url_parser)


target_compile_options(${COMPONENT_LIB} PRIVATE 
        -Wno-error=unused-label 
        -Wno-error=return-type 
        -Wno-error=missing-braces 
        -Wno-error=pointer-sign 
        -Wno-error=parentheses
)