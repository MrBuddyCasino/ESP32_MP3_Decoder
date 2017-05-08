#
# Component Makefile
#

# if Bluetooth is disabled in menuconfig,
# the BT libs are missing from the buildpath
ifdef CONFIG_BT_SPEAKER_MODE
COMPONENT_ADD_INCLUDEDIRS := . include
COMPONENT_SRCDIRS := .
else
COMPONENT_ADD_INCLUDEDIRS :=
COMPONENT_ADD_LDFLAGS :=
COMPONENT_SRCDIRS :=
endif
