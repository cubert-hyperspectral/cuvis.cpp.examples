#!/bin/bash

# Usage: 
# example app path 				[string] E.g. ./03_exportMeasurement_cpp.exe for windows or ./03_exportMeasurement_cpp for linux
# user settings directory 		[string] Default is ../../../sample_data/set_examples/settings
# sessionfile (.cu3s)			[string] Default is ../../../sample_data/set_examples/set0_single/single.cu3s
# user plugin file (.xml) 		[string] Default is ../../../sample_data/set_examples/plugin/ref/cai.xml
# name of export directory 		[string] The name of your output folder realtive to the example app path

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ##########LINUX##########
	./03_exportMeasurement_cpp											\
	"../../../sample_data/set_examples/settings"						\
	"../../../sample_data/set_examples/set0_single/single.cu3s" 		\
	"../../../sample_data/set_examples/plugin/ref/cai.xml"				\
	"export"															\
	
else
    ##########WINDOWS##########
	./03_exportMeasurement_cpp.exe 										\
	"../../../sample_data/set_examples/settings"						\
	"../../../sample_data/set_examples/set0_single/single.cu3s" 		\
	"../../../sample_data/set_examples/plugin/ref/cai.xml"				\
	"export"															\
	
fi


