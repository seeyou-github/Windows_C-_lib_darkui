@echo off
setlocal

if not exist build mkdir build

set CXX=g++
set CXXFLAGS=-std=c++17 -DUNICODE -D_UNICODE -DNOMINMAX -DWIN32_LEAN_AND_MEAN -Iinclude
set LDFLAGS=-municode -mwindows -lcomctl32 -ldwmapi -luxtheme -lgdi32

echo Building lib_darkui toolbar demo...
%CXX% demo\demo_toolbar.cpp src\toolbar.cpp src\button.cpp src\table.cpp src\combobox.cpp src\progress.cpp src\scrollbar.cpp src\slider.cpp src\tab.cpp %CXXFLAGS% -o build\darkui_toolbar_demo.exe %LDFLAGS%
if errorlevel 1 goto error

echo Build succeeded: build\darkui_toolbar_demo.exe
goto end

:error
echo Build failed
exit /b 1

:end
endlocal
