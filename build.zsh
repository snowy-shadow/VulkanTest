#!/bin/zsh

BuildDir="build"

case "$1" in
	"")
	cmake -B "$BuildDir" -S . -G Ninja  
	cmake --build "$BuildDir"
	;;

	"clean")
	cmake --build "$BuildDir" --target clean
	rm -rf "$BuildDir"
	;;
esac
