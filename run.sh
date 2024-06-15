#!/bin/bash

BuildDir="build_linux"

case "$1" in
	"")
		cmake -B "$BuildDir" -S . -G Ninja  
		;;

	"build")
		cmake -B "$BuildDir" -S . -G Ninja  
		cmake --build "$BuildDir"
		;;

	"fbuild")
		cmake --fresh -B "$BuildDir" -S . -G Ninja  
		cmake --build "$BuildDir"
		;;
		
	"clean")
		cmake --build "$BuildDir" --target clean
		rm -rf "$BuildDir"
		;;
esac
