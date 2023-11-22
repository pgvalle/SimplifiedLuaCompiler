@echo off

:: what a big deal to just store a command output in a variable
:: One more reason to not like windows
dir /B *.osz > TEMP_FILE
set /p SOURCE_FILES= < TEMP_FILE
del TEMP_FILE
:: compile
g++ %SOURCE_FILES% -o slc
