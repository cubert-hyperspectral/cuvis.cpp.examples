#!/bin/bash

# Usage: 
# example app path 				[string] E.g. ./Example_7_Record_Video_From_SessionFile_cpp.exe for windows or ./Example_7_Record_Video_From_SessionFile_cpp for linux	
# user settings directory 		[string] Default is ../settings
# sessionfile		 			[string] Default is ../SDK_Training_Example_Data/WinterUlm_X20P.cu3s
# name of recording	directory	[string] The name of your output folder relative to the example app path
# exposure time 				[int   ] exposure (integration) time in ms 
# auto exposure					[bool  ] 1 is "on", 0 is "off"; if "on" the aquisition will start with the given exposure time and then adjust on the fly 
# target fps					[double] Frames Per Second that shall be recorded. 

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ##########LINUX##########
	./Example_7_Record_Video_From_SessionFile_cpp 									\
	"../settings" 							\
	"../SDK_Training_Example_Data/WinterUlm_X20P.cu3s"		\
	"video01"																\
	100																		\
	0																		\
	2																		\
	
else
    ##########WINDOWS##########
	./Example_7_Record_Video_From_SessionFile_cpp.exe 										\
	"../settings"									\
	"../SDK_Training_Example_Data/WinterUlm_X20P.cu3s"	\
	"video01"																		\
	100																				\
	0																				\
	2																				\
	
fi
