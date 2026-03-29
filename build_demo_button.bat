@echo off
setlocal

if not exist build mkdir build

set CXX=g++
set CXXFLAGS=-std=c++17 -DUNICODE -D_UNICODE -Iinclude
set LDFLAGS=-municode -mwindows -lcomctl32 -ldwmapi -luxtheme

echo Building lib_darkui button demo...
%CXX% demo\demo_button.cpp src\button.cpp src\table.cpp src\combobox.cpp %CXXFLAGS% -o build\darkui_button_demo.exe %LDFLAGS%
if errorlevel 1 goto error

echo Build succeeded: build\darkui_button_demo.exe
goto end

:error
echo Build failed
exit /b 1

:end
endlocal
