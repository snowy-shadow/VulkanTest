@echo off

set BuildDir=build

IF "%~1"=="" (
	cmake "-B" "%BuildDir%" "-S" "." "-G" "Ninja"
	cmake "--build" "%BuildDir%"
) ELSE IF "%~1"=="clean" (
	cmake "--build" "%BuildDir%" "--target" "clean"
	@RD /S /Q %BuildDir%
	del "*.ilk"
	del "*.pdb"
)
