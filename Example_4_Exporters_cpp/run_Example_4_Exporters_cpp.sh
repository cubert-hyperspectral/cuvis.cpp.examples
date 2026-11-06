#!/bin/bash

# Usage: 
# example app path 				[string] E.g. ./Example_4_Exporters_cpp.exe for windows or ./Example_4_Exporters_cpp for linux
# user settings directory 		[string] Default is ../settings
# sessionfile (.cu3s)			[string] Default is ../SDK_Training_Example_Data/Drone.cu3s (demo data, Ultris X20P with pan sensor)
# measurement index			[int   ] Default is 2 (in Drone.cu3s, 0 and 1 are copies of the Dark and White references)
# user plugin file (.xml) 		[string] Default is ../plugins/00_RGB.xml
# name of export directory 		[string] The name of your output folder relative to the example app path

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ##########LINUX##########
	./Example_4_Exporters_cpp											\
	"../settings"						\
	"../SDK_Training_Example_Data/Drone.cu3s" 		\
	2																	\
	"../plugins/00_RGB.xml"				\
	"export"															\
	
else
    ##########WINDOWS##########
	./Example_4_Exporters_cpp.exe 										\
	"../settings"						\
	"../SDK_Training_Example_Data/Drone.cu3s" 		\
	2																	\
	"../plugins/00_RGB.xml"				\
	"export"															\
	
fi


