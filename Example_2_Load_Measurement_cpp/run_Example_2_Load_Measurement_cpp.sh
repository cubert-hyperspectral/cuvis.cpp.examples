#!/bin/bash

# Usage:
# example app path 				[string] E.g. ./01_loadMeasurement_cpp.exe for windows or ./01_loadMeasurement_cpp for linux	
# user settings directory 		[string] Default is ../../../sample_data/set_examples/settings
# sessionfile (.cu3s)			[string] Default is ../../../sample_data/set_examples/set0_single/single.cu3s

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ##########LINUX##########
	./01_loadMeasurement_cpp										\
	"../../../sample_data/set_examples/settings"					\
	"../../../sample_data/set_examples/set0_single/single.cu3s"	\

else
    ##########WINDOWS##########
	./01_loadMeasurement_cpp.exe 									\
	"../../../sample_data/set_examples/settings"					\
	"../../../sample_data/set_examples/set0_single/single.cu3s"	\

fi
