#!/bin/zsh

case "$1" in
	"")
	cmake -B build -S . -G Ninja  
	cmake --build build
	;;

	"clean")
	cmake --build build --target clean
	;;
esac
