#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <errno.h>

#define MAX_ENTRIES 1000
#define MAX_CHAR_LEN 10
#define MAX_TRANS_LEN 30

struct Dictionary  {
	wchar_t character[MAX_CHAR_LEN];
	char transliteration[MAX_TRANS_LEN];
};

struct Dictionary dict[MAX_ENTRIES];
int dictSize = 0;

void loadDictionary(const char* filename) {
	FILE* file = fopen(filename, "r");
	if (file == NULL) {perror("Error opening file"); exit(1);}

	wchar_t wline[256];
	while (fgetws(wline, sizeof(wline)/sizeof(wline[0]), file) && dictSize < MAX_ENTRIES) {
		wchar_t* comma = wcschr(wline, L',');

		if (comma) {
			*comma = L'\0';
			wcsncpy(dict[dictSize].character, wline, MAX_CHAR_LEN - 1);
			dict[dictSize].character[MAX_CHAR_LEN - 1] = L'\0';

			char* trans = (char*)(comma + 1);
			strncpy(dict[dictSize].transliteration, trans, MAX_TRANS_LEN - 1);
			dict[dictSize].transliteration[MAX_TRANS_LEN - 1] = '\0';
			dict[dictSize].transliteration[strcspn(dict[dictSize].transliteration, "\n")] = '\0';
			dictSize++;
		}
	}
	fclose(file);
	printf("Loaded %d entries into the dictionary.\n", dictSize);
}

const char* findTransliteration(wchar_t ch) {
	printf("Looking for %lc", ch);
	for (int i = 0; i < dictSize; i++) {
		if (wcscmp(dict[i].character, &ch) == 0) {
			return dict[i].transliteration;
		}
	}
	return NULL;
}

void transliterateText(const wchar_t* input) {
	for (int i = 0; input[i] != L'\0'; i++) {
		const char* trans = findTransliteration(input[i]);
		if (trans != NULL) {
			printf("%s", trans);
		} else {
			printf("%lc", input[i]);
		}
	}
	printf("\n");
}

int main() {
	setlocale(LC_ALL, "en_US.UTF-8");

	loadDictionary("transliteration.csv");

	wchar_t input[1000];
	while (1) {
		printf("Enter Chinese text (or 'q' to quit): ");
		if (fgetws(input, sizeof(input) / sizeof(input[0]), stdin) == NULL) {
			break;
		}
		input[wcscspn(input, L"\n")] = 0;  // Remove newline

		if (wcscmp(input, L"q") == 0) {
			break;
		}

		transliterateText(input);
	}

	return 0;
}
