#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>

enum process_status {
    CHILD_PROC, PARENT_PROC, ERR_PROC
};

struct find_result {
    enum process_status stat;
    int i;
};

long long xorN(FILE* file, int n) {
    long long res = 0;
    long long buf = 0;
    char* buf_bytes = (char*)&buf;
    size_t size_in_bits = 1;
    size_t size;
    int read_bytes;

    for (int i = 0; i < n; i++) {
        size_in_bits *= 2;
    }
    if (size_in_bits == 4) {
        read_bytes = fread(buf_bytes, 1, 1, file);
        while (read_bytes == 1) {
            res = res ^ (buf & ((1 << 4) - 1));
            res = res ^ (buf >> 4);
            read_bytes = fread(buf_bytes, 1, 1, file);
        }
        res = res & ((1 << size_in_bits) - 1);
    } else {
        size = size_in_bits / 8;
        read_bytes = fread(buf_bytes, 1, size, file);
        while (read_bytes == size) {
            res = res ^ buf;
            buf = 0;
            read_bytes = fread(buf_bytes, 1, size, file);
        }
        res = (res ^ buf);
    }
    return res;
}

int xorN_for_all(int argc, char* argv[]) {
    int n;
    if (!sscanf(argv[argc - 1], "xor%d", &n)) {
        printf("flag usage: xorN\n");
        return 1;
    }
    if ((n < 2) || (n > 6)) {
        printf("N must be from 2 to 6\n");
        return 1;
    }
    
    FILE* file;
    for (int i = 1; i <= argc - 2; i++) {
        file = fopen(argv[i], "r");
        if (file == NULL) {
            printf("%s fail to open\n", argv[i]);
        } else {
            printf("%s %llx\n", argv[i], xorN(file, n));
            fclose(file);
        }
    }
    return 0;
}

int copy_file(char* file_name, int i) {
    FILE* file = fopen(file_name, "r");
    int j = strlen(file_name) - 1;
    char* new_name = malloc(sizeof(char) * (j + 12));
    char* file_name_copy = malloc(sizeof(char) * (j + 1));
    while ((file_name[j] != '.') && (j > 0)) {
        j--;
    }
    strcpy(file_name_copy, file_name);
    file_name_copy[j] = 0;
    sprintf(new_name, "%s%d.%s", file_name_copy, i, &(file_name_copy[j + 1]));
    FILE* copy = fopen(new_name, "w");

    char buf;
    while(!feof(file)) {
        fread(&buf, 1, 1, file);
        fwrite(&buf, 1, 1, copy);
    }

    fclose(file);
    fclose(copy);
}

enum process_status copyN_for_all(int argc, char* argv[]) {
    int n;
    if(!sscanf(argv[argc - 1], "copy%d", &n)) {
        printf("invalid number of copies\n");
        return PARENT_PROC;
    }
    
    for(int i = 1; i <= n; i++) {
        int pid = fork();
        if (pid < 0) {
            return ERR_PROC;
        }
        if (pid == 0) {
            for (int j = 1; j <= argc - 2; j++) {
                copy_file(argv[j], i);
            }
            return CHILD_PROC;
        }
    }
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }
    return PARENT_PROC;
}

int count_mask(FILE* file, unsigned int mask) {
    unsigned int num;
    int counter = 0;
    fread(&num, 4, 1, file);
    while (!feof(file)) {
        printf(">%x %ld\n", num, sizeof(int));
        if ((num & mask) == mask) {
            counter++;
        }
        fread(&num, 4, 1, file);
    }
    return counter;
}

int mask_for_all(int argc, char* argv[]) {
    unsigned int mask;
    if (!sscanf(argv[argc - 1], "%x", &mask)) {
        printf("invalid mask\n");
        return 1;
    }

    FILE* file;
    int res;
    for (int i = 1; i <= argc - 3; i++) {
        file = fopen(argv[i], "rb");
        if (file == NULL) {
            printf("%s fail to open\n", argv[i]);
        } else {
            printf("%s %d\n", argv[i], count_mask(file, mask));
            fclose(file);
        }
    }
}

int find_pattern (char* file_name, char* pattern) {
    FILE* f = fopen(file_name, "r");
    if (!f) {
        printf("fail to open file %s", file_name);
        return 1;
    }
    int c;
    int i = 0;
    int pattern_len = strlen(pattern);
    char* queue = malloc(sizeof(char) * (pattern_len+1));
    int queue_start = 0;
    for (int j = 0; j < pattern_len; j++) {
        c = fgetc(f);
        if (c != EOF) {
            queue[j] = c;
        } else {
            return 0;
        }
    }
    queue[pattern_len] = '\0';
    while (c != EOF) {
        if (i == pattern_len) {
            return 1;
        } else if (pattern[i] == queue[(queue_start + i) % pattern_len]) {
            i++;
        } else {
            c = fgetc(f);
            queue[queue_start] = c;
            queue_start = (queue_start + 1) % pattern_len;
            i = 0;
        }
    }
    fclose(f);
    return 0;
}

struct find_result find_for_all (int argc, char* argv[]) {
    int n = argc - 3;
    for (int i = 1; i <= n; i++) {
        int pid = fork();
        if (pid < 0) {
            return (struct find_result){ERR_PROC, -1};
        }
        if (pid == 0) {
            if(find_pattern(argv[i], argv[argc - 1])) {
                return (struct find_result){CHILD_PROC, i};
            } else {
                return (struct find_result){CHILD_PROC, 0};
            }
        }
    }
    int flag = 0;
    int status;
    for (int i = 0; i < n; i++) {
        wait(&status);
        if (WIFEXITED(status)) {
            flag += WEXITSTATUS(status);
        }
    }
    return (struct find_result){PARENT_PROC, flag};
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <file1> ... <flag>\n", argv[0]);
        return 1;
    }
    char* flag = argv[argc - 1];
    if (!strncmp(argv[argc - 1], "xor", 3)) {
        xorN_for_all(argc, argv);
    } else if (!strcmp(argv[argc - 2], "mask") && (argc >= 4)) {
        mask_for_all(argc, argv);
    } else if (!strncmp(argv[argc - 1], "copy", 4)) {
        enum process_status res = copyN_for_all(argc, argv);
        if (res == ERR_PROC) {
            printf("fail fo fork");
        } else if (res == CHILD_PROC) {
            return 0;
        }
    } else if (!strcmp(argv[argc - 2], "find") && (argc >= 4)) {
        struct find_result res = find_for_all(argc, argv);
        if (res.stat == ERR_PROC) {
            printf("fail fo fork");
        } else if (res.stat == CHILD_PROC) {
            if (res.i > 0) {
                printf("%s\n", argv[res.i]);
                return 1;
            }
            return 0;
        } else {
            if (res.i == 0) {
                printf("not found\n");
                return 0;
            }
        }
    } else {
        printf("unknown command\n");
    }
}