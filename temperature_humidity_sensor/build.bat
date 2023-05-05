@echo off
set KEIL_PATH=C:\Keil\UV3\Uv3.exe
set PROJECT_PATH=.

%KEIL_PATH% -b "%PROJECT_PATH%\8051.Uv2" -o "build.log"