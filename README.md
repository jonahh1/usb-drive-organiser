# Simple & Portable Program Launcher
 
A small, portable, easy to use launcher for programs stored on a usb/portable drive. The launcher consists of itself and only needs a `config.txt` file beside it.
<p align="center">
 <img width="50%" alt="README image" src="https://github.com/user-attachments/assets/51688e89-1be6-4f2e-923c-ed95e29a5926" />
</p>

## Configuration
 
The `config.txt` tells the launcher where the programs are. It also has fields for the theme & appearance. Here's an example `config.txt` file:
```txt
programs(../../programs/) {
	MobaXTerm/MobaXterm_Personal_25.4.exe
	libresprite/libresprite.exe
	DOSBox-0.74-3/DOSBox.exe
}
windowRadius:     12
windowBorderSize: 2
windowPadding:    8

itemRadius:     6
itemBorderSize: 2
itemPadding:    4

itemCloseGap: 1
itemSize:     32

windowBGColour:      #241c36
windowBorderColour:  #eebb80
itemBorderOffColour: #dd8080
itemBorderOnColour:  #aa3080
```
Notes & caveats (because the parser for this is 30 lines of C code)
- `programs(../){...}` must come first
- the parameter in `programs()` is the default/starting folder
- colours are 6 digit hex only (`#rrggbb`)
- other than programs, all fields are optional
- no error reporting, sorry

## Building
Running `build.bat <debug|release> [run]` will build the program and include all assets inside the exe. The only difference between `debug` & `release` is where it looks for its `config.txt`, *(`assets/config.txt` when using `debug`, and `config.txt` when using `release`)*. the `run` argument is optional and just starts the exe once built.
When building it will create a `src/bin` folder to store the asset file data as generated C code.

## Development 
I wrote this in C using [raylib](https://github.com/raysan5/raylib), [clay](https://github.com/nicbarker/clay), and the ~Microslop~ *Microsoft* Windows API. Almost all code & assets are my own (excluding `lib/*` & `src/clayrl.c`). Apart from that it's only the Windows API code that's probably more AI than me.
