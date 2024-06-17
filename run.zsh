#!/bin/zsh

BuildDir="build_mac"

case "$1" in
	"")
		cmake -B "$BuildDir" -S . -G Ninja  
		;;

	"build")
		cmake -B "$BuildDir" -S . -G Ninja  
		cmake --build "$BuildDir"
		;;
		
	"buildf")
		cmake --fresh -B "$BuildDir" -S . -G Ninja  
		cmake --build "$BuildDir"
		;;

	"clean")
		cmake --build "$BuildDir" --target clean
		rm -rf "$BuildDir"
		;;
esac
