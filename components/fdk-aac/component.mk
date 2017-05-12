
CPPFLAGS += -Wno-error=maybe-uninitialized -Wno-maybe-uninitialized -Wno-error=unused-label -Wno-unused-label -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-variable 

COMPONENT_ADD_INCLUDEDIRS += libAACdec/include \
					 libFDK/include \
					 libMpegTPDec/include \
					 libPCMutils/include \
					 libSBRdec/include \
					 libSYS/include 
					 
COMPONENT_SRCDIRS += libAACdec/src \
					 libFDK/src \
					 libMpegTPDec/src \
					 libPCMutils/src \
					 libSBRdec/src \
					 libSYS/src 
					 
