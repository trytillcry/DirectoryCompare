@echo off
setlocal enabledelayedexpansion

for /R %1 %%i in (*) do (
	echo %%~dpi^|%%~nxi^|%%~zi
)
endlocal

