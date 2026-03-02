#ifndef MAIN_H
#define MAIN_H

#define STRLEN 256

// CLAYRL.C
#define RAYLIB_STATIC
#include "../lib/raylib.h"
#include "../lib/clay.h"
static inline Clay_Dimensions Raylib_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData);
void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands, Font* fonts, int *maxW, int *maxH);
// UTILS.C

int strsub(char *str, char search, char replace);
size_t exeNameStart(char *str);
size_t exeNameLen(char *str);
// CONFIG.C
typedef struct {
	char programNames[16][STRLEN];
	int programsCount;

	int windowRadius;
	int windowBorderSize;
	int windowPadding;

	int itemRadius;
	int itemBorderSize;
	int itemPadding;

	int itemCloseGap;
	int itemSize;

	uint32_t windowBGColour;
	uint32_t windowBorderColour;
	uint32_t itemBorderOffColour;
	uint32_t itemBorderOnColour;

} Configuration;
void readConfig(const char *path);

// WIN.C


// EXEICON.C
int LoadDLLs();
void startProcess(char *path);
wchar_t* char_to_wchar(const char* str);
unsigned char* LoadExeIconData(const char *exePath, int *w, int *h);

#endif