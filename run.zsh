#!/bin/zsh

BuildDir="build_mac"

  # Set the C/C++ compiler

case "$1" in
	"")
		cmake -B "$BuildDir" -S . -G Ninja  
		;;

	"build")
		cmake -D CMAKE_C_COMPILER="/opt/homebrew/opt/llvm/bin/clang" -D CMAKE_CXX_COMPILER="/opt/homebrew/opt/llvm/bin/clang++" -B "$BuildDir" -S . -G Ninja  
		cmake --build "$BuildDir"
		;;
		
	"buildf")
		cmake --fresh -D CMAKE_C_COMPILER="/opt/homebrew/opt/llvm/bin/clang" -D CMAKE_CXX_COMPILER="/opt/homebrew/opt/llvm/bin/clang++" -B "$BuildDir" -S . -G Ninja  
		cmake --build "$BuildDir"
		;;

	"clean")
		cmake --build "$BuildDir" --target clean
		rm -rf "$BuildDir"
		;;
esac
