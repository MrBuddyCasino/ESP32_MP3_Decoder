#
# Main Makefile. This is basically the same as a component makefile.
#
# This Makefile should, at the very least, just include $(SDK_PATH)/make/component_common.mk. By default, 
# this will take the sources in the src/ directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#

# define makros which take wifi AP name and password from environment variables
# CFLAGS += -D WIFI_AP_NAME=\"$AP_NAME\" -D WIFI_AP_PASS=\"$AP_PASS\" 
