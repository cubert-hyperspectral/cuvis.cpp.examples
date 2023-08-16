#!/bin/bash

# Usage: 
# example app path 				[string] E.g. ./07_recordVideo_cpp.exe for windows or ./07_recordVideo_cpp for linux	
# user settings directory 		[string] Default is C:/Program Files/cuvis/user/settings for windows or /etc/cuvis/user/settings for linux
# sessionfile		 			[string] Default is C:/Program Files/cuvis/sdk/sample_data/set0_lab/x20p_calib_color.cu3s for windows or /etc/cuvis/sdk/sample_data/set0_lab/x20p_calib_color.cu3s for linux
# name of recording	directory	[string] The name of your output folder realtive to the example app path
# exposure time 				[int   ] exposure (integration) time in ms 
# auto exposure					[bool  ] 1 is "on", 0 is "off"; if "on" the aquisition will start with the given exposure time and then adjust on the fly 
# target fps					[double] Frames Per Second that shall be recorded. 

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ##########LINUX##########
	./07_recordVideoFromSessionfile_cpp 									\
	"/etc/cuvis/settings" 													\
	"/etc/cuvis/sdk/sample_data/set0_lab/x20p_calib_color.cu3s"				\
	"video01"																\
	100																		\
	0																		\
	2																		\
	
else
    ##########WINDOWS##########
	./07_recordVideoFromSessionfile_cpp.exe 								\
	"C:/Program Files/cuvis/user/settings"							\
	"C:/Program Files/cuvis/sdk/sample_data/set0_lab/x20p_calib_color.cu3s"	\
	"video01"																\
	100																		\
	0																		\
	2																		\
	
fi
