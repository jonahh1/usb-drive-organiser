#include <stdio.h>
#include <string.h>

#include "main.h"

Configuration Config;

void readConfig(const char *path) {
	
	Config.windowRadius = 10;
	Config.windowBorderSize = 2;
	Config.windowPadding = 8;

	Config.itemRadius = 5;
	Config.itemBorderSize = 2;
	Config.itemPadding = 4;

	Config.itemCloseGap = 2;
	Config.itemSize = 32;

	Config.windowBGColour = 0x241c36;
	Config.windowBorderColour = 0xeebb80;
	Config.itemBorderOffColour = 0xdd8080;
	Config.itemBorderOnColour = 0xaa3080;

	char programsFolder[STRLEN];
	int programsFolderStrlen;
	char fieldName[STRLEN*4];
	int fieldNameLen;
	int c = 0;
  
	FILE* fptr;

	fptr = fopen(path, "r");
	fscanf(fptr, "programs(%[^)]) { ", &programsFolder[0]);
	programsFolderStrlen = strlen(&programsFolder[0]);
	Config.programsCount = 0;
	while ((c = getc(fptr)) != '}') {
		strcpy(&Config.programNames[Config.programsCount][0], &programsFolder[0]);
		Config.programNames[Config.programsCount][programsFolderStrlen] = c;

		fscanf(fptr, "%[^\n] ", &Config.programNames[Config.programsCount][programsFolderStrlen+1]);

		strsub(&Config.programNames[Config.programsCount][0], '/', '\\');
		
		Config.programsCount++;
	}
	#define READ_FIELD(fmt, x) (strcmp(&fieldName[0], #x) == 0) {fscanf(fptr, fmt, &Config.x);}
	while ((c = fscanf(fptr, " %[^:]: ", &fieldName[0])) != EOF) {

		if      READ_FIELD("%d ", windowRadius)
		else if READ_FIELD("%d ", windowBorderSize)
		else if READ_FIELD("%d ", windowPadding)

		else if READ_FIELD("%d ", itemRadius)
		else if READ_FIELD("%d ", itemBorderSize)
		else if READ_FIELD("%d ", itemPadding)
		
		else if READ_FIELD("%d ", itemCloseGap)
		else if READ_FIELD("%d ", itemSize)
		
		else if READ_FIELD("#%x ", windowBGColour)
		else if READ_FIELD("#%x ", windowBorderColour)
		else if READ_FIELD("#%x ", itemBorderOffColour)
		else if READ_FIELD("#%x ", itemBorderOnColour)
	}
	#undef READ_FIELD

	
	/*for (size_t i = 0; i < Config.programsCount; i++)
	{
		strcpy(&Config.programNames[i][0], &programsFolder[0]);
		fscanf(fptr, "\t%[^\n]\n", &Config.programNames[i][0]+programsFolderStrlen);
		strsub(&Config.programNames[i][0], '/', '\\');
		//getExePixelData(&programNames[i][0]);
	}*/
	fclose(fptr);
}