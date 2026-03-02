@ECHO off


gcc bin2cstr.c -o bin2cstr.exe

mkdir src\bin
bin2cstr.exe IconPNG  assets/icon.png  src/bin/icon.png.c
bin2cstr.exe ClosePNG assets/close.png src/bin/close.png.c
bin2cstr.exe LogoPNG  assets/logo.png  src/bin/logo.png.c
bin2cstr.exe --externHeader src/bin/files.h src/bin/*.c

windres lib/resource.rc -O coff -o lib/resource.o

for /f %%A in ('powershell -NoProfile -Command "'%1'.ToUpper()"') do set arg_upper=%%A

gcc -O2 -Bc1 -DUSB_ORG_%arg_upper% main.c src/*.c src/bin/*.c lib/resource.o -o main.exe -static -L"lib/" -lraylib -lwinmm -mwindows

if "%2" == "run" (
	main.exe
)