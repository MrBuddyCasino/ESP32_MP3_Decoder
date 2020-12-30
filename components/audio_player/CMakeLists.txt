idf_component_register(SRCS 
                            "audio_player.c"
                    INCLUDE_DIRS "include"
                    REQUIRES audio_renderer fdk-aac_decoder fifo)

