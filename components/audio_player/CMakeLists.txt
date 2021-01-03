idf_component_register(SRCS 
                            "audio_player.c"
                    INCLUDE_DIRS "include"
                    REQUIRES common 
                    PRIV_REQUIRES fifo controls libfaad_decoder fdk-aac_decoder mp3_decoder audio_renderer )

