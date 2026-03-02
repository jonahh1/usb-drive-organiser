#include <stdio.h>

#include "src/utils.c"
#include "src/config.c"

extern Configuration Config;

int main(void) {
	readConfig("assets/config.txt");

	printf("programs:\n");
	for (size_t i = 0; i < Config.programsCount; i++)
	{
		printf("  [%d]: %s\n", i, &Config.programNames[i][0]);
	}
	printf("\n");
	
	printf("colours:\n");
	printf("  win bg:          #%x\n", Config.windowBGColour);
	printf("  win border:      #%x\n", Config.windowBorderColour);
	printf("  item border off: #%x\n", Config.itemBorderOffColour);
	printf("  item border on:  #%x\n", Config.itemBorderOnColour);
	printf("\n");
	
	printf("windowRadius:      %d\n", Config.windowRadius );
	printf("windowBorderSize:  %d\n", Config.windowBorderSize);
	printf("windowPadding:     %d\n", Config.windowPadding);
	printf("\n");

	printf("itemRadius:        %d\n", Config.itemRadius);
	printf("itemBorderSize:    %d\n", Config.itemBorderSize);
	printf("itemPadding:       %d\n", Config.itemPadding);
	printf("\n");

	printf("itemCloseGap:      %d\n", Config.itemCloseGap);
	return 0;
}