#!/bin/bash

# Language option.
LANGUAGE=$1
# GUI text editor.
TEXTEDITOR=gnome-text-editor
# Text file.
fragmentFile="$HOME/.fragview/frag-example.frag"

# Select shading language.
case $LANGUAGE in
	"glsl-fragment")
	    renderer="--renderer-opengl"
	    fileType="-f"
        sampleFile="/usr/share/fragview/samples/glsl/wave.frag"
		;;
	"hlsl-fragment")
	    echo "Not supported!"
	    exit 1
		;;
    "glsl-compute")
        renderer="--renderer-opengl"
	    fileType="-C"
        sampleFile="/usr/share/fragview/samples/glsl/compute.comp"
        ;;
    "cl-compute")
        renderer="--renderer-opencl"
        fileType="-C"
        sampleFile="/usr/share/fragview/samples/clc/sobeedgedectection.cl"
        ;;
	*)
        exit 1
		;;
esac

# Create directory and file.
mkdir -p $(dirname "$fragmentFile")
touch $fragmentFile

# Create default shader.
cat $sampleFile > $fragmentFile

# Start editor and viewer
$TEXTEDITOR $fragmentFile &
fragview $fileType $fragmentFile $renderer