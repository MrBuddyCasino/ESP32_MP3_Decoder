idf_component_register(SRCS 
                            "audio_renderer.c"
                    INCLUDE_DIRS "include"
                    REQUIRES common
                    PRIV_REQUIRES MerusAudio audio_player)

