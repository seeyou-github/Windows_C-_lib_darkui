@echo off
setlocal

if not exist build mkdir build

set CXX=g++
set CXXFLAGS=-std=c++17 -DUNICODE -D_UNICODE -DNOMINMAX -DWIN32_LEAN_AND_MEAN -Iinclude
set LDFLAGS=-municode -mwindows -lcomctl32 -ldwmapi -luxtheme -lgdi32

echo Building lib_darkui edit demo...
%CXX% demo\demo_edit.cpp src\edit.cpp src\button.cpp src\combobox.cpp %CXXFLAGS% -o build\darkui_edit_demo.exe %LDFLAGS%
if errorlevel 1 goto error

echo Build succeeded: build\darkui_edit_demo.exe
goto end

:error
echo Build failed
exit /b 1

:end
endlocal
