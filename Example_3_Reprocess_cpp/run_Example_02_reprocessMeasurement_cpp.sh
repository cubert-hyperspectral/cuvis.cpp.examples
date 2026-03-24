#!/bin/bash

# Usage: 
# example app path 				[string] E.g. .02_reprocessMeasurement.exe for windows or ./02_reprocessMeasurement_cpp for linux	
# user settings directory 		[string] Default is ../../../sample_data/set_examples/settings
# measurement file (.cu3s)		[string] Default is ../../../sample_data/set0_single/single_raw.cu3s
# dark file (.cu3s)				[string] Default is ../../../sample_data/set0_single/single_dark.cu3s
# white file (.cu3s)			[string] Default is ../../../sample_data/set0_single/single_white.cu3s
# distance file (.cu3s)			[string] Default is ../../../sample_data/set0_single/single_distance.cu3s
# name of output directory 		[string] The name of your output folder realtive to the example app path

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ##########LINUX##########
	./02_reprocessMeasurement_cpp									\
	"../../../sample_data/set_examples/settings"					\
	"../../../sample_data/set_examples/set0_single/single_raw.cu3s" 		\
	"../../../sample_data/set_examples/set0_single/single_dark.cu3s"		\
	"../../../sample_data/set_examples/set0_single/single_white.cu3s"		\
	"../../../sample_data/set_examples/set0_single/single_distance.cu3s"	\
	"reprocessed"													\
	
else
    ##########WINDOWS##########
	./02_reprocessMeasurement_cpp.exe 								\
	"../../../sample_data/set_examples/settings"					\
	"../../../sample_data/set_examples/set0_single/single_raw.cu3s" 		\
	"../../../sample_data/set_examples/set0_single/single_dark.cu3s"		\
	"../../../sample_data/set_examples/set0_single/single_white.cu3s"		\
	"../../../sample_data/set_examples/set0_single/single_distance.cu3s"	\
	"reprocessed"													\
	
fi
