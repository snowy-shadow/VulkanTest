@echo off

set BuildDir=build_win

IF "%~1"=="" (
	cmake "-B" "%BuildDir%" "-S" "." "-G" "Ninja"
) ELSE IF "%~1"=="build" (
	cmake "-B" "%BuildDir%" "-S" "." "-G" "Ninja"
	cmake "--build" "%BuildDir%"
) ELSE IF "%~1"=="buildf" (
	cmake "--fresh" "-B" "%BuildDir%" "-S" "." "-G" "Ninja"
	cmake "--build" "%BuildDir%"
) ELSE IF "%~1"=="clean" (
	cmake "--build" "%BuildDir%" "--target" "clean"
	@RD /S /Q %BuildDir%
	del "*.ilk"
	del "*.pdb"
)
