# 86BoxLauncher Windows Installer

These instructions can be used to create an installation package for Windows users.

## Requirements

* C++17 compiler or newer
* CMake 3.13 or newer
* Qt 5 or Qt 6
* Inno Setup Compiler

## Building the installer

Start the Command Prompt and run the following commands. Please edit commands to match your setup.

````cmd
"C:\SDKs\Qt\6.7.0\msvc2019_64\bin\qtenv2.bat"
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
cd C:\Projects\86BoxLauncher
cmake -B build .
cmake --build build --config Release
cmake --install build --config Release --prefix pkgs\windows\deploy
cd pkgs\windows\deploy
windeployqt --release --no-system-d3d-compiler --no-system-dxc-compiler ^
	--no-opengl-sw --exclude-plugins qtuiotouchplugin 86BoxLauncher.exe
cd ..
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" 86boxlauncher.iss
````

