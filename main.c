
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>

#define RAYLIB_STATIC
#include "lib/raylib.h"
#define CLAY_IMPLEMENTATION
#include "lib/clay.h"
#include "src/main.h"
#include "src/bin/files.h"

#define RGBA(x) (Clay_Color) { (x)>>24, ((x)>>16)&0xff, ((x)>>8)&0xff, (x)&0xff }
#define RGB(x) (Clay_Color) { ((x)>>16)&0xff, ((x)>>8)&0xff, (x)&0xff, 255 }

const Clay_Color COLOR_LIGHT = RGB(0xf1f1f1);
const Clay_Color COLOR_RED = RGB(0xff0080);
const Clay_Color COLOR_ORANGE = RGB(0xff8000);

static inline Clay_Dimensions Raylib_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
	// Measure string size for Font
	Clay_Dimensions textSize = { 0 };

	float maxTextWidth = 0.0f;
	float lineTextWidth = 0;
	int maxLineCharCount = 0;
	int lineCharCount = 0;

	float textHeight = config->fontSize;
	Font* fonts = (Font*)userData;
	Font fontToUse = fonts[config->fontId];
	// Font failed to load, likely the fonts are in the wrong place relative to the execution dir.
	// RayLib ships with a default font, so we can continue with that built in one. 
	if (!fontToUse.glyphs) {
		fontToUse = GetFontDefault();
	}

	float scaleFactor = config->fontSize/(float)fontToUse.baseSize;

	for (int i = 0; i < text.length; ++i, lineCharCount++)
	{
		if (text.chars[i] == '\n') {
			maxTextWidth = fmax(maxTextWidth, lineTextWidth);
			maxLineCharCount = CLAY__MAX(maxLineCharCount, lineCharCount);
			lineTextWidth = 0;
			lineCharCount = 0;
			continue;
		}
		int index = text.chars[i] - 32;
		if (fontToUse.glyphs[index].advanceX != 0) lineTextWidth += fontToUse.glyphs[index].advanceX;
		else lineTextWidth += (fontToUse.recs[index].width + fontToUse.glyphs[index].offsetX);
	}

	maxTextWidth = fmax(maxTextWidth, lineTextWidth);
	maxLineCharCount = CLAY__MAX(maxLineCharCount, lineCharCount);

	textSize.width = maxTextWidth * scaleFactor + (lineCharCount * config->letterSpacing);
	textSize.height = textHeight;

	return textSize;
}
void HandleClayErrors(Clay_ErrorData errorData) {
	printf("%s", errorData.errorText.chars);
}

// Layout config is just a struct that can be declared statically, or inline
Clay_ElementDeclaration sidebarItemConfig = (Clay_ElementDeclaration) {
	.layout = {
		.sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) }
	},
	.backgroundColor = COLOR_ORANGE
};

// Re-useable components are just normal functions
void SidebarItemComponent() {
	CLAY({
		.id=0,
		.layout = {
			.sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) }
		},
		.backgroundColor = COLOR_ORANGE
	}) {
		// children go here...
	}
}

extern Configuration Config;

static inline void OnHoverFunc(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData) {

}

int main(void) {

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TRANSPARENT);
	InitWindow(400, 450, "USB Organiser");
	SetTargetFPS(30);
	Image logoImg = LoadImageFromMemory(".png", LogoPNGPtr, LogoPNGLen);
	//Texture2D icon = LoadTextureFromImage(iconImg);
	
	SetWindowIcon(logoImg);
	
	//int frames;
	Image iconImg = LoadImageFromMemory(".png", IconPNGPtr, IconPNGLen);
	Texture2D icon = LoadTextureFromImage(iconImg);
	UnloadImage(iconImg);
	iconImg = LoadImageFromMemory(".png", ClosePNGPtr, ClosePNGLen);
	Texture2D closeIcon = LoadTextureFromImage(iconImg);
	UnloadImage(iconImg);

	#if defined(USB_ORG_RELEASE)
		readConfig("config.txt");
	#elif defined(USB_ORG_DEBUG)
		readConfig("assets/config.txt");
	#endif

	Clay_ElementId programButtonIds[Config.programsCount];
	Texture2D programIcons[Config.programsCount];
	for (size_t i = 0; i < Config.programsCount; i++) {
		programButtonIds[i] = CLAY_SID(((Clay_String){
			.isStaticallyAllocated = true,
			.length = exeNameLen(Config.programNames[i]),
			.chars = Config.programNames[i] + exeNameStart(Config.programNames[i]),
		}));
		int w, h;
		unsigned char *pixData = LoadExeIconData(Config.programNames[i], &w, &h);
		if (pixData != NULL) {
			programIcons[i] = LoadTextureFromImage((Image){
				.data = pixData,
				.width = w,
				.height = h,
				.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
				.mipmaps = 1, 
			});
			free(pixData);
		}
		else {
			programIcons[i] = icon;
		}

	}
	

	uint64_t totalMemorySize = Clay_MinMemorySize();
	Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));

	Clay_Initialize(arena, (Clay_Dimensions) { 400, 450 }, (Clay_ErrorHandler) { HandleClayErrors });
	
	Font fonts = GetFontDefault();

	Clay_SetMeasureTextFunction(Raylib_MeasureText, &fonts);
	Clay_OnHover(OnHoverFunc, 0);

	bool doCloseWindow = false;
	while (!WindowShouldClose() && !doCloseWindow)
	{
		SetWindowPosition(GetMonitorWidth(0)/2-GetScreenWidth()/2, GetMonitorHeight(0)/3);
	
		// Optional: Update internal layout dimensions to support resizing
		Clay_SetLayoutDimensions((Clay_Dimensions) { GetScreenWidth(), GetScreenHeight() });
		// Optional: Update internal pointer position for handling mouseover / click / touch events - needed for scrolling & debug tools
		Clay_SetPointerState((Clay_Vector2) { GetMouseX(), GetMouseY() }, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
		// Optional: Update internal pointer position for handling mouseover / click / touch events - needed for scrolling and debug tools
		Clay_UpdateScrollContainers(true, (Clay_Vector2){GetMouseWheelMoveV().x,GetMouseWheelMoveV().y}, GetFrameTime());

		// All clay layouts are declared between Clay_BeginLayout and Clay_EndLayout
		Clay_BeginLayout();

		#define COL_SIDEBAR_BG RGB(Config.windowBGColour)
		#define COL_BORDER_A RGB(Config.windowBorderColour)
		#define COL_BORDER_B RGB(Config.itemBorderOffColour)
		#define COL_BORDER_C RGB(Config.itemBorderOnColour)
		#define PAD 8
		#define RAD 10
		// An example of laying out a UI with a fixed width sidebar and flexible width main content
		CLAY({
			.id = CLAY_ID("OuterContainer"),
			.layout = {
				.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
				.padding = CLAY_PADDING_ALL(0),
				.childGap = 0
			},
			.backgroundColor = RGBA(0)
		}) {
			CLAY({.id=CLAY_ID("SideBar"),
				.layout = {
					.layoutDirection = CLAY_TOP_TO_BOTTOM,
					.sizing = { .height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_FIT(0) },
					.padding = CLAY_PADDING_ALL(Config.windowPadding),
					.childGap = Config.windowPadding,
					.layoutDirection = CLAY_LEFT_TO_RIGHT,
				},
				.backgroundColor = COL_SIDEBAR_BG,
				.cornerRadius = CLAY_CORNER_RADIUS(Config.windowRadius),
				.border = {
					.color = COL_BORDER_A,
					.width = CLAY_BORDER_OUTSIDE(Config.windowBorderSize),
				}
			}) {
				// CLAY({.id=CLAY_ID("ProfilePictureOuter"), .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(16), .childGap = 16, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = COLOR_RED }) {
				// 	CLAY({.id=CLAY_ID("ProfilePicture"),.layout = { .sizing = { .width = CLAY_SIZING_FIXED(60), .height = CLAY_SIZING_FIXED(60) }} }) {}
				// 	CLAY_TEXT(CLAY_STRING("Clay - UI Library"), CLAY_TEXT_CONFIG({ .fontId=0, .fontSize = 24, .textColor = {255, 255, 255, 255} }));
				// }

				// Standard C code like loops etc work inside components
				for (int i = 0; i < Config.programsCount; i++) {
					//SidebarItemComponent();
					CLAY({
						.id = programButtonIds[i],
						.layout = {
							.sizing = {
								.width = CLAY_SIZING_FIT(0),
								.height = CLAY_SIZING_FIT(0),
							},
							.padding = CLAY_PADDING_ALL(Config.itemPadding),
						},
						.aspectRatio = 1,
						.cornerRadius = CLAY_CORNER_RADIUS(Config.itemRadius),
						.border = {
							.color = Clay_Hovered() ? COL_BORDER_C : COL_BORDER_B,
							.width = CLAY_BORDER_ALL(Config.itemBorderSize),
						},
						
					}) {
						if (Clay_Hovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
							startProcess(&Config.programNames[i][0]);
						}
						CLAY({
							.layout = {
								.sizing = {
									.width = CLAY_SIZING_FIXED(Config.itemSize),
									.height = CLAY_SIZING_GROW(0),
								},
							},
							.aspectRatio = 1,
							.image = {.imageData = &programIcons[i]},
						}) {}
					}
				}
				if (Config.itemCloseGap > 0)
					CLAY({ .id=CLAY_ID("Spacer"),
						.layout = {
							.sizing = {
								.width = CLAY_SIZING_FIXED(
									Config.itemCloseGap*(Config.itemSize + Config.itemPadding*2) - Config.windowPadding
								)
							}
						},
					}) {}
				
				CLAY({
					.id = CLAY_ID("close button"),
					.layout = {
						.sizing = {
							.width = CLAY_SIZING_FIT(0),
							.height = CLAY_SIZING_FIT(0),
						},
						.padding = CLAY_PADDING_ALL(Config.itemPadding),
					},
					.aspectRatio = 1,
					.cornerRadius = CLAY_CORNER_RADIUS(Config.itemRadius),
					.border = {
						.color = Clay_Hovered() ? COL_BORDER_C : COL_BORDER_B,
						.width = CLAY_BORDER_ALL(Config.itemBorderSize),
					},
					
				}) {
					if (Clay_Hovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
						doCloseWindow = true;
					}
					CLAY({
						.layout = {
							.sizing = {
								.width = CLAY_SIZING_FIXED(Config.itemSize),
								.height = CLAY_SIZING_GROW(0),
							},
						},
						.aspectRatio = 1,
						.image = {.imageData = &closeIcon},
					}) {}
				}

			}
		}
		// All clay layouts are declared between Clay_BeginLayout and Clay_EndLayout
		Clay_RenderCommandArray renderCommands = Clay_EndLayout();
		BeginDrawing();
		ClearBackground((Color){0,0,0,0});
		int w,h;
		Clay_Raylib_Render(renderCommands, &fonts, &w, &h);
		SetWindowSize(w, h);
		// char StR[1024];
		// sprintf(&StR[0], "%d, %d", w, h);
		// for (int y = -1; y <= 1; y++)
		// 	for (int x = -1; x <= 1; x++)
		// 		DrawText(&StR[0], x,y, 20, WHITE);
		// DrawText(&StR[0], 0,0, 20, BLACK);
		
		EndDrawing();
	}
	UnloadFont(fonts);
	UnloadTexture(icon);
	for (size_t i = 0; i < Config.programsCount; i++) {
		UnloadTexture(programIcons[i]);
	}
	UnloadImage(logoImg);
	
	
	CloseWindow();

	return 0;
}

// LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
// {
// 	//readConfig("config.txt");
// 	readConfig("assets/config.txt");
	
// 	setWindowTitle("i <3 C");
// 	setWindowProcess(WindowProc);

// 	if (createWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow) == 0) {
// 		runWindow();
// 	}

// 	return 0;
// }


// LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// {
// 	switch (uMsg) {
		
// 		case WM_CREATE: {
// 			CreateWindow(
// 				"STATIC",
// 				"shortcuts with relative paths seem like a pain...",
// 				WS_VISIBLE | WS_CHILD,
// 				24, 10, 500, 24,
// 				hwnd,
// 				NULL,
// 				(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
// 				NULL
// 			);
// 			CreateWindow(
// 				"STATIC",
// 				"so i learned how to use the windows api (barely) to make a window with 2 buttons",
// 				WS_VISIBLE | WS_CHILD,
// 				24, 34, 500, 24,
// 				hwnd,
// 				NULL,
// 				(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
// 				NULL
// 			);
// 			CreateWindow(
// 				"STATIC",
// 				"and it's also configurable",
// 				WS_VISIBLE | WS_CHILD,
// 				24, 58, 500, 24,
// 				hwnd,
// 				NULL,
// 				(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
// 				NULL
// 			);
// 			for (int i = 0; i < programsCount; i++)
// 			{
// 				CreateWindow(
// 					"BUTTON",
// 					&programNames[i][0]+exeNameStart(&programNames[i][0]),
// 					WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
// 					24, 100+28*i, 250, 24,
// 					hwnd,
// 					BUTTON_ID(i),
// 					(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
// 					NULL
// 				);
// 			}
// 		}
// 		return 0;
		
// 		case WM_COMMAND: {
// 			int wmId = LOWORD(wParam);
// 			int wmEvent = HIWORD(wParam);

// 			if ((wmId>=BUTTON_ID_START && wmId<BUTTON_ID_START+programsCount) && wmEvent == BN_CLICKED) {
// 				// chaneg to program dir
// 				char s[STRLEN];
// 				int exeStart = exeNameStart(&programNames[wmId - BUTTON_ID_START][0]);
// 				getcwd(s, STRLEN);
// 				programNames[wmId - BUTTON_ID_START][exeStart - 1] = 0;
// 				chdir(&programNames[wmId - BUTTON_ID_START][0]);
// 				programNames[wmId - BUTTON_ID_START][exeStart - 1] = '\\';
// 				system(&programNames[wmId-BUTTON_ID_START][0] + exeStart);
// 				chdir(&s[0]);

// 				// change back to old dir
// 				// STARTUPINFO si = {sizeof(si)};
// 				// PROCESS_INFORMATION pi;
// 				// if (!CreateProcess(NULL, &programNames[wmId-BUTTON_ID_START][0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
// 				// 	printf("Failed to start process: %lu\n", GetLastError());
// 			}
// 		}
// 		return 0;

// 		case WM_DESTROY: {
// 			PostQuitMessage(0);
// 		}
// 		return 0;
// 	}

// 	return DefWindowProc(hwnd, uMsg, wParam, lParam);
// }