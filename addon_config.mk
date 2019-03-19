meta:
    ADDON_NAME = ofxLibRealsense2P
    ADDON_DESCRIPTION = let's realsense
    ADDON_AUTHOR = horristic
    ADDON_TAGS = "realsense" "librealsense"

vs:
	# x64
	ADDON_LIBS += libs\realsense\lib\x64\realsense2.lib
	ADDON_DLLS_TO_COPY += copy_dll_to_bin\x64\realsense2.dll

	# Win32
	#ADDON_LIBS += libs\realsense\lib\x86\realsense2.lib
	#ADDON_DLLS_TO_COPY += copy_dll_to_bin/x86/realsense2.dll