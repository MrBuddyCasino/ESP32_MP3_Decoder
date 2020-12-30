idf_component_register(SRCS 
                            "ui.c"
                    INCLUDE_DIRS "include"
                    REQUIRES nvs_flash user_driver inih_port vector)

