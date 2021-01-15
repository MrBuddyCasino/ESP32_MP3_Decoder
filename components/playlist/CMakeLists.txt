idf_component_register(SRCS 
                            "playlist.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES common vector inih_port
                    EMBED_FILES playlist.pls)

