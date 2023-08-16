#!/bin/bash

# Usage: 
# example app path 				[string] E.g. ./05_recordSingleImages_cpp.exe for windows or ./05_recordSingleImages_cpp for linux	
# user settings directory 		[string] Default is C:/Program Files/cuvis/user/settings for windows or /etc/cuvis/user/settings for linux
# factory directory 			[string] Default is C:/Program Files/cuvis/factory for windows or /etc/cuvis/factory for linux
# name of recording directory 	[string] The name of your recording folder realtive to the example app path
# exposure time 				[int   ] exposure (integration) time in ms 
# number of images to record 	[int   ] the number of images you want to record 

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ##########LINUX##########
	./05_recordSingleImages_cpp						\
	"/etc/cuvis/user/settings"						\
	"/etc/cuvis/factory" 							\
	"images"										\
	100												\
	10												\
	
else
    ##########WINDOWS##########
	./05_recordSingleImages_cpp.exe 				\
	"C:/Program Files/cuvis/user/settings"			\
	"C:/Program Files/cuvis/factory" 				\
	"images"										\
	100												\
	10												\
	
fi
