idf_component_register(SRCS 
                            "spiram_fifo.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES http main)


target_compile_options(${COMPONENT_LIB} PRIVATE 
        -Wno-error=unused-label 
        -Wno-error=return-type 
        -Wno-error=missing-braces 
        -Wno-error=pointer-sign 
        -Wno-error=parentheses
)