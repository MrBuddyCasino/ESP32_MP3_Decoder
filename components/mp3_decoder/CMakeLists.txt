idf_component_register(SRCS 
                            "mp3_decoder.c"
                    INCLUDE_DIRS "include"
                    REQUIRES common mad audio_renderer audio_player)

