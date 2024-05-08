#!/bin/zsh

BuildDir="build"

case "$1" in
	"")
		cmake -B "$BuildDir" -S . -G Ninja  
		cmake --build "$BuildDir"
		;;

	"clangdb")
		cmake -B "$BuildDir" -S . -G Ninja  
		;;

	"clean")
		cmake --build "$BuildDir" --target clean
		rm -rf "$BuildDir"
		;;
esac
