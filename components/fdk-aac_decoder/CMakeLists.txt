idf_component_register(SRCS 
                            "fdk_aac_decoder.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES common fdk-aac libm4a audio_renderer)

