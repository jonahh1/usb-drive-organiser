#include <stddef.h>

#include "main.h"

int strsub(char *str, char search, char replace) {
	int count;
	for (size_t i = 0; str[i] != 0; i++) {
		if (str[i] == search) {
			str[i] = replace;
			count++;
		}
	}
	return count;
}

size_t exeNameStart(char *str) {
	size_t i = 0;
	for (; str[i] != 0; i++);
	for (; i != 0 && str[i-1] != '\\'; i--);
	return i;
}
size_t exeNameLen(char *str) {
	size_t i = 0;
	size_t j = 0;
	for (; str[i] != 0; i++);
	for (j=i; j != 0 && str[j-1] != '\\'; j--);
	return i-j;
}


