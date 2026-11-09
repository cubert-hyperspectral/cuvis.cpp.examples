#!/bin/bash

# Usage: 
# example app path 				[string] E.g. ./Example_3_Reprocess_cpp.exe for windows or ./Example_3_Reprocess_cpp for linux	
# user settings directory 		[string] Default is ../settings
# measurement file (.cu3s)		[string] Default is ../SDK_Training_Example_Data/single_raw.cu3s (demo data, Ultris X20)
# dark file (.cu3s)				[string] Default is ../SDK_Training_Example_Data/single_raw.cu3s
# white file (.cu3s)			[string] Default is ../SDK_Training_Example_Data/single_raw.cu3s
# distance file (.cu3s)			[string] Default is ../SDK_Training_Example_Data/single_raw.cu3s
# name of output directory 		[string] The name of your output folder relative to the example app path

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ##########LINUX##########
	./Example_3_Reprocess_cpp									\
	"../settings"					\
	"../SDK_Training_Example_Data/single_raw.cu3s" 		\
	"../SDK_Training_Example_Data/single_raw.cu3s"		\
	"../SDK_Training_Example_Data/single_raw.cu3s"		\
	"../SDK_Training_Example_Data/single_raw.cu3s"	\
	"reprocessed"													\
	
else
    ##########WINDOWS##########
	./Example_3_Reprocess_cpp.exe 								\
	"../settings"					\
	"../SDK_Training_Example_Data/single_raw.cu3s" 		\
	"../SDK_Training_Example_Data/single_raw.cu3s"		\
	"../SDK_Training_Example_Data/single_raw.cu3s"		\
	"../SDK_Training_Example_Data/single_raw.cu3s"	\
	"reprocessed"													\
	
fi
