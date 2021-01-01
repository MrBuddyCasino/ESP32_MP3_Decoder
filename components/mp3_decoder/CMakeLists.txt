idf_component_register(SRCS 
                            "mp3_decoder.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES common mad audio_renderer audio_player fifo)

