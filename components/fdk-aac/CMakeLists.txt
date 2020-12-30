idf_component_register(SRC_DIRS "libAACdec/src" "libFDK/src" "libSYS/src" "libMpegTPDec/src" "libSBRdec/src"
                        "libPCMutils/src"
                    INCLUDE_DIRS "libAACdec/include" "libFDK/include" "libSYS/include" "libMpegTPDec/include" 
                    "libSBRdec/include"
                    "libSBRdec/src"
                        "libPCMutils/include"
                    )
target_compile_options(${COMPONENT_LIB} PRIVATE 
    -Wno-error=maybe-uninitialized 
    -Wno-maybe-uninitialized 
    -Wno-error=unused-label 
    -Wno-unused-label 
    -Wno-unused-function 
    -Wno-unused-but-set-variable 
    -Wno-unused-variable
)