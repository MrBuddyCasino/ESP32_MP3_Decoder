if(CONFIG_BT_SPEAKER_MODE)
    idf_component_register(SRC_DIRS . 
                    INCLUDE_DIRS "include"
                    REQUIRES nvs_flash bt audio_renderer)
endif()

