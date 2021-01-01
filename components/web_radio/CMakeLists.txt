idf_component_register(SRCS 
                            "web_radio.c"
                    INCLUDE_DIRS "include"
                    REQUIRES audio_player playlist
                    PRIV_REQUIRES vector  http controls common audio_renderer nghttp url_parser)

