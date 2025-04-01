#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

typedef enum authorize_mod {
    SIGN_IN, SIGN_UP, ERR_MOD, AUTHORIZED, WRONG_PINCODE, UNKNOWN_LOGIN, EXISTING_LOGIN, INVALID_LOGIN, INVALID_PINCODE, EXIT
} authorize_mod;

typedef struct User {
    char login[7];
    int pincode;
    char limit;
} User;

authorize_mod hello() {
    printf("Please, enter 1 for sign in, 2 for sign up or 3 to exit\n");
    int c;
    scanf("%d", &c);
    if (c == 1) {
        return SIGN_IN;
    } else if (c == 2) {
        return SIGN_UP;
    } else if (c == 3) {
        return EXIT;
    } else {
        return ERR_MOD;
    }
}

int validate_login (char* login) {
    int n = strlen(login);
    if (n > 6) {
        return 0;
    }
    for (int i = 0; i < n; i++) {
        if (!isalnum(login[i])) return 0;
    }
    return 1;
}

int validate_pincode (int pincode) {
    return (pincode >= 0) && (pincode <= 100000);
}

int authorize(authorize_mod* mod, FILE* users, char* limit) {
    User enter_user;
    User cur_user;
    enter_user.login[6] = 0;
    cur_user.login[6] = 0;
    cur_user.limit = -1;
    
    if (*mod == SIGN_IN) {
        printf("enter login: ");
        scanf("%s", enter_user.login);

        if (!validate_login(enter_user.login)) {
            *mod = INVALID_LOGIN;
            fseek(users, 0, SEEK_SET);
            return (*mod != AUTHORIZED);
        }

        printf("enter pin code: ");
        scanf("%d", &enter_user.pincode);

        if (!validate_pincode(enter_user.pincode)) {
            *mod = INVALID_PINCODE;
            fseek(users, 0, SEEK_SET);
            return (*mod != AUTHORIZED);
        }

        fread(&cur_user, sizeof(User), 1, users);    

        while (!feof(users) && strcmp(cur_user.login, enter_user.login)) {
            fread(&cur_user, sizeof(User), 1, users);
        }

        if (!strcmp(cur_user.login, enter_user.login)) {
            if (enter_user.pincode == cur_user.pincode) {

                *mod = AUTHORIZED;
            } else {
                
                *mod = WRONG_PINCODE;
            }
        } else {
            
            *mod = UNKNOWN_LOGIN;
        }
    } else if (*mod == SIGN_UP) {
        printf("enter login: ");
        scanf("%s", enter_user.login);

        if (!validate_login(enter_user.login)) {
            *mod = INVALID_LOGIN;
            fseek(users, 0, SEEK_SET);
            return (*mod != AUTHORIZED);
        }

        fread(&cur_user, sizeof(User), 1, users);

        while (!feof(users) && strcmp(cur_user.login, enter_user.login)) {
            //printf(">%s", cur_user.login);
            fread(&cur_user, sizeof(User), 1, users);
        }

        if (!strcmp(cur_user.login, enter_user.login)) {
            
            *mod = EXISTING_LOGIN;
        } else {
            printf("enter pin code: ");
            scanf("%d", &enter_user.pincode);

            if (!validate_pincode(enter_user.pincode)) {
                *mod = INVALID_PINCODE;
                fseek(users, 0, SEEK_SET);
                return (*mod != AUTHORIZED);
            }

            fwrite(&enter_user, sizeof(User), 1, users);

            *mod = AUTHORIZED;
        }
    }
    *limit = cur_user.limit;
    fseek(users, 0, SEEK_SET);
    return (*mod != AUTHORIZED);
}

int sanctions(char* command, FILE* users) {
    char login[7];
    char number;
    int key;
    if(sscanf(command, "Sanctions %s %hhd", login, &number) != 3) {
        printf("invalid command format\n");
        return 1;
    }

    //printf("(%s %d)\n", login, number);

    if (!validate_login(login)) {
        printf("invalid login\n");
        return 1;
    }

    printf("enter key: ");
    scanf("%d", &key);

    
    if (key != 12345) {
        printf("incorrect key");
        return 1;
    }

    User cur_user;
    fread(&cur_user, sizeof(User), 1, users);

    while (!feof(users) && strcmp(cur_user.login, login)) {
        fread(&cur_user, sizeof(User), 1, users);
    }

    if (strcmp(cur_user.login, login)) {
        printf("unknown login\n");
        return 1;
    }

    cur_user.limit = number;

    fseek(users, -sizeof(User), SEEK_CUR);
    fwrite(&cur_user, sizeof(User), 1, users);
    printf("limits edited\n");
    fseek(users, 0, SEEK_SET);
    return 0;
}

int print_time() {
    time_t now;
    struct tm* local;
    time(&now);
    local = localtime(&now);
    printf("Time: %02d:%02d:%02d\n", local->tm_hour, local->tm_min, local->tm_sec);
    return 0;
}

int print_date() {
    time_t now;
    struct tm* local;
    time(&now);
    local = localtime(&now);
    printf("Time: %02d:%02d:%04d\n", local->tm_mday, local->tm_mon + 1, local->tm_year + 1900);
    return 0;
}

int print_howmuch(char* command) {
    struct tm input_time;
    time_t cur_seconds, input_seconds;
    double result;
    char* time_str = &(command[8]);
    if (time_str[19] != ' ') {
        printf("Invalid command.\n");
        return 1;
    }

    time_str[19] = 0;
    char* flag = &(time_str[20]);

    if (strptime(time_str, "%d:%m:%Y %H:%M:%S", &input_time) == NULL) {
        printf("Invalid time format. Use 'DD:MM:YYYY HH:MM:SS'.\n");
        return 1;
    }

    input_seconds = mktime(&input_time);
    time(&cur_seconds);
    double diff_seconds = difftime(cur_seconds, input_seconds);

    if (strcmp(flag, "-s") == 0) {
        result = diff_seconds;
        printf("Elapsed time: %.0f seconds\n", result);
    } else if (strcmp(flag, "-m") == 0) {
        result = diff_seconds / 60;
        printf("Elapsed time: %.2f minutes\n", result);
    } else if (strcmp(flag, "-h") == 0) {
        result = diff_seconds / 3600;
        printf("Elapsed time: %.2f hours\n", result);
    } else if (strcmp(flag, "-y") == 0) {
        result = diff_seconds / (3600 * 24 * 365);
        printf("Elapsed time: %.2f years\n", result);
    } else {
        printf("Invalid flag: %s. Use -s, -m, -h, or -y.\n", flag);
    }

}

void print_message (authorize_mod mod) {
    switch (mod)
    {
    case ERR_MOD: printf("invalid mod"); break;
    case AUTHORIZED: printf("authorized\n"); break;
    case WRONG_PINCODE: printf("wrong pincode\n"); break;
    case UNKNOWN_LOGIN: printf("unknown login, check your login or sign up\n"); break;
    case EXISTING_LOGIN: printf("login already exists\n"); break;
    case INVALID_LOGIN: printf("invalid login\n"); break;
    case INVALID_PINCODE: printf("invalid pincode\n"); break;
    default: break;
    }
}

void print_menu(int limit) {
    printf("----------------\n");
    printf("Commands:\n");
    if (limit < 0) {
        printf("You have not limits\n");
    } else {
        printf("You can enter %d commands before log out\n", limit);
    }
    printf("----------------\n");
    printf("Time\n");
    printf("Date\n");
    printf("Howmuch <time> flag\n");
    printf("Logout\n");
    printf("Sanctions <username> <number>\n");
    printf("----------------\n");
}

int get_line(char** line) {
    *line = malloc(sizeof(char) * 10);
    int line_size = 10;
    char c = getchar();
    if ((c == '\n')) {
        c = getchar();
    }
    int i = 0;
    while ((c != '\n') && (c != EOF)) {
        (*line)[i] = c;
        c = getchar();
        i++;
        if (i >= line_size) {
            line_size *= 2;
            *line = realloc(*line, sizeof(char) * line_size);
        }
    }
    (*line)[i] = '\0';
    return 1;
}

int main() {
    authorize_mod mod = hello();
    FILE* users = fopen("users.db", "a");
    fclose(users);
    users = fopen("users.db", "rb+");
    char* command;
    char limit;

    authorize(&mod, users, &limit);
    print_message(mod);
    while (mod != EXIT) {
        if (mod != AUTHORIZED) {
            mod = hello();
            authorize(&mod, users, &limit);
            print_message(mod);
        } else {
            print_menu(limit);
            get_line(&command);
            //printf("<%s>\n", command);
            if (!strcmp(command, "Time")) {
                print_time();
                limit--;
            } else if (!strcmp(command, "Date")) {
                print_date();
                limit--;
            } else if (!strncmp(command, "Howmuch ", 8)) {
                print_howmuch(command);
                limit--;
            } else if (!strncmp(command, "Sanctions", 9)) {
                sanctions(command, users);
                limit--;
            } else if (!strcmp(command, "Logout")) {
                mod = SIGN_IN;
            }
            
            if (limit == 0) {
                mod = SIGN_IN;
            } else if (limit < 0) {
                limit = -1;
            }
        }
    }

    free(command);
    return 0;
}