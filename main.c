#include <stdio.h>
#include <string.h>
#include <windows.h>

typedef void (*FunctionType)(void); // 함수 포인터 타입 정의

void parseJagalFile(const char *filePath) {
    FILE *file = fopen(filePath, "r");
    char line[1024];
    HMODULE hModule = NULL;

    if (!file) {
        printf("Error opening file\n");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        char command[10], param[1024];

        if (sscanf(line, "%s \"%[^\"]\";", command, param) == 2) {
            if (strcmp(command, "dllload") == 0) {
                hModule = LoadLibraryA(param);
                if (!hModule) {
                    printf("Failed to load %s\n", param);
                } else {
                    printf("Loaded %s\n", param);
                }
            } else if (strcmp(command, "freedll") == 0) {
                if (hModule) {
                    FreeLibrary(hModule);
                    printf("Freed %s\n", param);
                }
            }
        } else if (sscanf(line, "call %s;", param) == 1) {
            if (hModule) {
                FunctionType function = (FunctionType)GetProcAddress(hModule, param);
                if (function) {
                    function();
                    printf("Called %s\n", param);
                } else {
                    printf("Function %s not found\n", param);
                }
            }
        }
    }

    if (hModule) {
        FreeLibrary(hModule);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file.jagal>\n", argv[0]);
        return 1;
    }

    parseJagalFile(argv[1]);
    return 0;
}
