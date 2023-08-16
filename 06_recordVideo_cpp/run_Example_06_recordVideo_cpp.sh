#!/bin/bash

# Usage: 
# example app path 				[string] E.g. ./06_recordVideo_cpp.exe for windows or ./06_recordVideo_cpp for linux	
# user settings directory 		[string] Default is C:/Program Files/cuvis/user/settings for windows or /etc/cuvis/user/settings for linux
# factory directory 			[string] Default is C:/Program Files/cuvis/factory for windows or /etc/cuvis/factory for linux
# name of recording	directory	[string] The name of your output folder realtive to the example app path
# exposure time 				[int   ] exposure (integration) time in ms 
# auto exposure					[bool  ] 1 is "on", 0 is "off"; if "on" the aquisition will start with the given exposure time and then adjust on the fly 
# target fps					[double] Frames Per Second that shall be recorded. 

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ##########LINUX##########
	./06_recordVideo_cpp 						\
	"/etc/cuvis/user/settings"					\
	"/etc/cuvis/factory" 						\
	"video01"									\
	100											\
	0											\
	2											\
	
else
    ##########WINDOWS##########
	./06_recordVideo_cpp.exe 					\
	"C:/Program Files/cuvis/user/settings"		\
	"C:/Program Files/cuvis/factory" 			\
	"video01"									\
	100											\
	0											\
	2											\
	
fi
