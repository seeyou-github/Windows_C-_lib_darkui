@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
set "ROOT_DIR=%SCRIPT_DIR%.."
set "DEMO_SRC_DIR=%ROOT_DIR%\demo\src"
set "LIB_SRC_DIR=%ROOT_DIR%\src"
set "INCLUDE_DIR=%ROOT_DIR%\include"
set "BUILD_DIR=%ROOT_DIR%\demo\build"

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

if "%CXX%"=="" set "CXX=g++"

set "CXXFLAGS=-std=c++17 -Wall -Wextra -municode -mwindows -I%INCLUDE_DIR%"
set "LDFLAGS=-municode -mwindows -lcomctl32 -ldwmapi -luxtheme -lgdi32"

echo Building lib_darkui listview demo...
%CXX% "%DEMO_SRC_DIR%\demo_listview.cpp" "%LIB_SRC_DIR%\listview.cpp" "%LIB_SRC_DIR%\themed_window_host.cpp" "%LIB_SRC_DIR%\combobox.cpp" "%LIB_SRC_DIR%\edit.cpp" "%LIB_SRC_DIR%\button.cpp" "%LIB_SRC_DIR%\static.cpp" %CXXFLAGS% -o "%BUILD_DIR%\darkui_listview_demo.exe" %LDFLAGS%
if errorlevel 1 exit /b 1

echo Build succeeded: "%BUILD_DIR%\darkui_listview_demo.exe"
