#!/bin/bash

# Usage:
# example app path 				[string] E.g. ./Example_6_Reference_Spectra_cpp.exe for windows or ./Example_6_Reference_Spectra_cpp for linux
# user settings directory 		[string] Default is ../settings
# sessionfile (.cu3s)			[string] Default is ../SDK_Training_Example_Data/WinterUlm_X20P.cu3s (demo data, must contain Dark and White references)
# name of output directory 		[string] The name of your output folder relative to the example app path

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ##########LINUX##########
	./Example_6_Reference_Spectra_cpp								\
	"../settings"													\
	"../SDK_Training_Example_Data/WinterUlm_X20P.cu3s"			\
	"reference_spectra"												\

else
    ##########WINDOWS##########
	./Example_6_Reference_Spectra_cpp.exe 							\
	"../settings"													\
	"../SDK_Training_Example_Data/WinterUlm_X20P.cu3s"			\
	"reference_spectra"												\

fi
