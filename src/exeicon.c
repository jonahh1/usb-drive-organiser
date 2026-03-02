#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

#define STRLEN 256

int LoadDLLs() {
	HMODULE lib = LoadLibrary("l ib\\raylib.dll");
	if (!lib) {
		lib = LoadLibrary("programs\\bin\\raylib.dll");
		if (!lib) {
			MessageBox(NULL, "Failed to load DLL", "Error", MB_OK);
			return 1;
		}
	}
	MessageBox(NULL, "Failed to load DLL", "Error", MB_OK);
	return 0;
}

size_t exeNameStart(char *str);

void startProcess(char *path) {
	char s[STRLEN];
	int exeStart = exeNameStart(path);
	getcwd(s, STRLEN);
	path[exeStart - 1] = 0;
	chdir(path);
	path[exeStart - 1] = '\\';
	STARTUPINFO si = {sizeof(si)};
	PROCESS_INFORMATION pi;
	CreateProcess(NULL, path + exeStart, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	//system(&programNames[wmId-BUTTON_ID_START][0] + exeStart);
	chdir(&s[0]);
}

wchar_t* char_to_wchar(const char* str)
{
	int size_needed = MultiByteToWideChar(
		CP_UTF8,			// Source encoding (change if needed)
		0,
		str,
		-1,
		NULL,
		0
	);

	wchar_t* wstr = (wchar_t*)malloc(size_needed * sizeof(wchar_t));

	MultiByteToWideChar(
		CP_UTF8,
		0,
		str,
		-1,
		wstr,
		size_needed
	);

	return wstr; // remember to free()
}
unsigned char* LoadExeIconData(const char *exePath, int *w, int *h) {
	wchar_t *ep = char_to_wchar(exePath);


	HICON iconHandle = NULL;
	UINT  iconId = 0;

	UINT count = PrivateExtractIconsW(
		ep, 0,
		32, 32,
		&iconHandle, &iconId,
		1,
		LR_LOADFROMFILE
	);
	if (iconHandle == NULL && count == 0) {
		printf("function failed");
		return NULL;
	}
	else if (iconHandle != NULL && count == 0xffffffff) {
		printf("file not found");
		return NULL;
	}
	ICONINFO iconInfo;
	if (!GetIconInfo(iconHandle, &iconInfo)) {
		printf("failed to get icon info");
		return NULL;
	}

	BITMAP color;
	GetObject(iconInfo.hbmColor, sizeof(BITMAP), &color);
	BITMAPINFO colorI = {0};
	colorI.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	colorI.bmiHeader.biWidth = color.bmWidth;
	colorI.bmiHeader.biHeight = -color.bmHeight; // negative for top-down
	colorI.bmiHeader.biPlanes = 1;
	colorI.bmiHeader.biBitCount = 32;
	colorI.bmiHeader.biCompression = BI_RGB;
	int imageSize =  color.bmWidth * color.bmHeight * 4;
	BYTE* pixels = malloc(imageSize);
	HDC hdc = GetDC(NULL);
	GetDIBits(
		hdc,
		iconInfo.hbmColor,
		0,
		color.bmHeight,
		pixels,
		&colorI,
		DIB_RGB_COLORS
	);
	
	BITMAP mask;
	GetObject(iconInfo.hbmMask, sizeof(mask), &mask);
	BITMAPINFO maskI = {0};
	maskI.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	maskI.bmiHeader.biWidth = color.bmWidth;
	maskI.bmiHeader.biHeight = -color.bmHeight;
	maskI.bmiHeader.biPlanes = 1;
	maskI.bmiHeader.biBitCount = 1;
	maskI.bmiHeader.biCompression = BI_RGB;
	int maskSize = ((color.bmWidth + 31) / 32) * color.bmHeight * 4;
	BYTE* maskData = malloc(maskSize);
	hdc = GetDC(NULL);
	GetDIBits(
		hdc,
		iconInfo.hbmMask,
		0,
		color.bmHeight,
		maskData,
		&maskI,
		DIB_RGB_COLORS
	);

	ReleaseDC(NULL, hdc);

	(*w) = color.bmWidth;
	(*h) = color.bmHeight;

	for (int i = 0; i < (*w) * (*h); i++) {

		pixels[i*4 + 0] ^= pixels[i*4 + 2];
		pixels[i*4 + 2] ^= pixels[i*4 + 0];
		pixels[i*4 + 0] ^= pixels[i*4 + 2];
		
		int byteIndex = (i/(*w)) * ((color.bmWidth + 31) / 32)*4 + ((i%(*w)) / 8);
		int bitIndex  = 7 - ((i%(*w)) % 8);
		BOOL transparent = (maskData[byteIndex] >> bitIndex) & 1;
		pixels[i*4 + 3] = 0xff * !transparent;
	}

	free(maskData);
	ReleaseDC(NULL, hdc);
	DeleteObject(iconInfo.hbmColor);
	DeleteObject(iconInfo.hbmMask);
	DestroyIcon(iconHandle);
	free(ep);
	return pixels;

	// Extract icon
	/*HICON hIcon = ExtractIconW(NULL, ep, 0);
	if (hIcon == NULL) {
		printf("Failed to extract icon\n");
		return (void*)0;
	}

	ICONINFO iconInfo;
	GetIconInfo(hIcon, &iconInfo);

	BITMAP bmp;
	GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bmp);

	int width = bmp.bmWidth;
	int height = bmp.bmHeight;

	printf("Icon size: %dx%d\n", width, height);

	// Prepare bitmap info for 32-bit RGBA
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;  // top-down bitmap
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	int imageSize = width * height * 4;
	BYTE *pixels = malloc(imageSize);

	HDC hdc = GetDC(NULL);

	GetDIBits(
		hdc,
		iconInfo.hbmColor,
		0,
		height,
		pixels,
		&bmi,
		DIB_RGB_COLORS
	);

	ReleaseDC(NULL, hdc);

	// pixels now contains BGRA data
	// Example: print first pixel
	printf("First pixel BGRA: %d %d %d %d\n",
		   pixels[0], pixels[1], pixels[2], pixels[3]);

	// If you need RGBA instead of BGRA:
	for (int i = 0; i < width * height; i++) {
		BYTE b = pixels[i*4 + 0];
		BYTE g = pixels[i*4 + 1];
		BYTE r = pixels[i*4 + 2];
		BYTE a = pixels[i*4 + 3];

		pixels[i*4 + 0] = r;
		pixels[i*4 + 1] = g;
		pixels[i*4 + 2] = b;
		pixels[i*4 + 3] = a;
	}
	(*w) = width;
	(*h) = height;

	// Cleanup
	DeleteObject(iconInfo.hbmColor);
	DeleteObject(iconInfo.hbmMask);
	DestroyIcon(hIcon);*/

}