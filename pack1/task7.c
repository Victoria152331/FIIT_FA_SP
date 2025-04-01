#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

int my_ls(const char *dirname) {
    struct dirent *entry;
    struct stat file_stat;
    DIR *dp = opendir(dirname);

    if (dp == NULL) {
        printf("fail fo open %s\n", dirname);
        return 1;
    }
    int dirname_len = strlen(dirname);
    char* filepath = malloc(sizeof(char) * (dirname_len + 1024));
    if (filepath == NULL) {
        printf("alloc fail\n");
        return 1;
    }
    int max_name_len = 1024;
    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        if (strlen(entry->d_name) >= max_name_len) {
            max_name_len = strlen(entry->d_name) + 1;
            if(realloc(filepath, sizeof(char) * (dirname_len + max_name_len)) == NULL) {
                printf("alloc fail\n");
                return 1;
            }
        }
        snprintf(filepath, dirname_len + max_name_len, "%s/%s", dirname, entry->d_name);

        if (stat(filepath, &file_stat) == -1) {
            continue;
        }
        
        printf("%s\n", entry->d_name);
    }
    closedir(dp);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <directory1> ...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        printf("Directory: %s\n", argv[i]);
        my_ls(argv[i]);
        printf("\n");
    }

    return 0;
}
