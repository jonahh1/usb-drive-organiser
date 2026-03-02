#include <stdio.h>
#include <string.h>
#include <windows.h>


int main(int argc, char *argv[]) {
	if (argc == 4 && strcmp(argv[1], "--externHeader")==0) {
		char *extPath = argv[2];
		char *cFiles = argv[3];
		char filePath[1024];
		size_t i=0;
		for (; cFiles[i]!=0; i++);
		for (; i > 0 && cFiles[i]!='/'; i--);
		i++;
		memcpy(filePath, cFiles, i);

		WIN32_FIND_DATA findFileData;

		HANDLE hFind = FindFirstFile(cFiles, &findFileData);

		if (hFind == INVALID_HANDLE_VALUE) {
			printf("FindFirstFile failed\n");
			return 1;
		}
		FILE *extFptr = fopen(&extPath[0], "w");
		fputs("#ifndef BINARY_FILE_DATA_H\n", extFptr);
		fputs("#define BINARY_FILE_DATA_H\n\n", extFptr);
		do {
			strcpy(&filePath[i], findFileData.cFileName);
			FILE *cFptr = fopen(&filePath[0], "r");
			char arrName[1024];
			fscanf(cFptr, "const char %[^[]", &arrName[0]);
			fclose(cFptr);
			int len = strlen(&arrName[0]);
			arrName[len-3] = 0;
			fprintf(extFptr, "extern const char *%sPtr;\n", &arrName[0]);
			fprintf(extFptr, "extern const int   %sLen;\n\n", &arrName[0]);
			
		} while (FindNextFile(hFind, &findFileData) != 0);

		fputs("#endif", extFptr);
		fclose(extFptr);
		FindClose(hFind);
		return 0;
		//FILE *outFptr = fopen(outPath, "w");
	}
	else if (argc < 4) {
		printf("\e[31m missing arguments.\e[0m\n");
		return 1;
	}

	const char *varName = argv[1];
	const char *inPath  = argv[2];
	const char *outPath = argv[3];


	FILE *inFptr = fopen(inPath, "rb");
	FILE *outFptr = fopen(outPath, "w");
	if (inFptr == NULL || outFptr == NULL) {
		printf("\e[31m unable to open input or output files.\e[0m\n");
		return 1;
	}
	
	fprintf(outFptr, "const char %sArr[] = {", varName);
	int chr = 0;
	size_t len = 0;
	while ((chr = fgetc(inFptr)) != EOF) {

		// newline every 16 characters
		if (len % 16 == 0) fputc(10, outFptr);

		// insert every valid byte
		if (chr >= 0 && chr <= 0xff)
			fprintf(outFptr, "0x%x%x,", chr/16, chr%16);
		
		// iterate length
		len++;
	}
	fprintf(outFptr, "\n};\n", varName, varName);
	fprintf(outFptr, "const char *%sPtr = &%sArr[0];\n", varName, varName);
	fprintf(outFptr, "const int   %sLen = %d;\n", varName, len);

	fclose(inFptr);
	fclose(outFptr);
	return 0;
	return 0;
	
	// const char *inPath    = "assets/logo.png";
	// const char *outPath   = "src/logo.png.c";
	// const char *outHeader = "const char LogoPngData[] = {";
	// const char *outFooter = "\n};\nunsigned long long LogoPngDataLen = %d;";
	// const char out[] = {
	// 	3,5,1,4,1
	// };
	// FILE *inFptr = fopen(inPath, "rb");
	// FILE *outFptr = fopen(outPath, "w");
	
	// if (inFptr == NULL || outFptr == NULL) {
	// 	return 1;
	// }
	
	// int c;
	// size_t iter = 0;
	// fputs(outHeader, outFptr);
	// while ((c = fgetc(inFptr)) != EOF) {
	// 	if (iter % 16 == 0)
	// 		fputc(10, outFptr);

	// 	if (c >= 0 && c <= 0xff)
	// 		fprintf(outFptr, "0x%x%x,", c/16, c%16);
	// 	iter++;
	// }
	// fprintf(outFptr, outFooter, iter);
	
	// fclose(inFptr);
	// fclose(outFptr);
	// return 0;
}