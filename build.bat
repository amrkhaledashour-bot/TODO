@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"
cmake -B build -G "NMake Makefiles" -DCMAKE_PREFIX_PATH=C:\Qt\6.6.2\msvc2019_64
cmake --build build
