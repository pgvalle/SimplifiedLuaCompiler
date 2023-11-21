@echo off

Dir /b source\*.cpp > %SOURCE_FILES%
g++ %SOURCE_FILES% -o slc
