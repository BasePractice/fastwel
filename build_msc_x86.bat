@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
rem call D:\VisualStudio\2017\Community\VC\Auxiliary\Build\vcvars32.bat

mkdir .build_msc_32
cd .build_msc_32
rem cmake .. -DCMAKE_BUILD_TYPE=Release -DARCH=x64 -G "NMake Makefiles"
cmake .. -DCMAKE_BUILD_TYPE=Debug -DARCH=32 -G "NMake Makefiles"
nmake
pause
