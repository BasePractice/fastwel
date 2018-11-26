@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
rem call D:\VisualStudio\2017\Community\VC\Auxiliary\Build\vcvars64.bat

mkdir .build_msc_64
cd .build_msc_64
rem cmake .. -DCMAKE_BUILD_TYPE=Release -DARCH=x64 -G "NMake Makefiles"
cmake .. -DCMAKE_BUILD_TYPE=Debug -DARCH=64 -G "NMake Makefiles"
nmake
pause
