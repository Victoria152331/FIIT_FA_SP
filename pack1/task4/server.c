#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 128

struct message {
    long msg_type;
    char msg_text[MSG_SIZE];
};

typedef enum Item {
    EMPTY, //means empty or position of boat
    WOLF, GOAT, CABBAGE
} Item;

typedef enum Location {
    LEFT, RIGHT
} Location;

struct cur_situation {
    Location loc[4];
    Item in_boat;
};

typedef enum Result {
    OK, WOLF_ATE_GOAT, GOAT_ATE_CABBAGE, CANNOT_TAKE, CANNOT_PUT, INVALID_PTR, INVALID_COMMAND
} Result;

Result take_item(struct cur_situation* situation, Item item) {
    if (!situation) {
        return INVALID_PTR;
    }

    if ((situation->in_boat == EMPTY) && (situation->loc[EMPTY] == situation->loc[item])) {
        situation->in_boat = item;
        return OK;
    } else {
        return CANNOT_TAKE;
    }
}

Result put_item(struct cur_situation* situation) {
    if (!situation) {
        return INVALID_PTR;
    }

    if (situation->in_boat != EMPTY) {
        situation->in_boat = EMPTY;
        return OK;
    } else {
        return CANNOT_PUT;
    }
}

Result move(struct cur_situation* situation) {
    if (!situation) {
        return INVALID_PTR;
    }

    if (situation->loc[EMPTY] == LEFT) {
        situation->loc[EMPTY] = RIGHT;
        situation->loc[situation->in_boat] = RIGHT;
    } else {
        situation->loc[EMPTY] = LEFT;
        situation->loc[situation->in_boat] = LEFT;
    }
    return OK;
}



Result check(struct cur_situation* situation) {
    if (!situation) {
        return INVALID_PTR;
    }

    if (situation->loc[GOAT] != situation->loc[EMPTY]) {
        if (situation->loc[GOAT] == situation->loc[WOLF]) {
            return WOLF_ATE_GOAT;
        }
        if (situation->loc[GOAT] == situation->loc[CABBAGE]) {
            return GOAT_ATE_CABBAGE;
        }
    }
    return OK;
}

Result init_situation(struct cur_situation* situation) {
    if (!situation) {
        return INVALID_PTR;
    }
    situation->loc[EMPTY] = LEFT;
    situation->loc[WOLF] = LEFT;
    situation->loc[GOAT] = LEFT;
    situation->loc[CABBAGE] = LEFT;
    situation->in_boat = EMPTY;
    return OK;
}

Result process_command(struct cur_situation* situation, char* command) {
    if(!strcmp(command, "take goat")) {
        return take_item(situation, GOAT);
    } else if (!strcmp(command, "take wolf")) {
        return take_item(situation, WOLF);
    } else if (!strcmp(command, "take cabbage")) {
        return take_item(situation, CABBAGE);
    } else if (!strcmp(command, "put")) {
        return put_item(situation);
    } else if (!strcmp(command, "move")) {
        return move(situation);
    } else {
        return INVALID_COMMAND;
    }
}

int send_answer(int msgid, long client_pid, Result res) {
    struct message msg;
    msg.msg_type = client_pid + 1;
    switch (res) {
    case OK:
        strcpy(msg.msg_text, "accepted");
        break;
    case WOLF_ATE_GOAT:
        strcpy(msg.msg_text, "wolf ate goat");
        break;
    case GOAT_ATE_CABBAGE:
        strcpy(msg.msg_text, "goat ate cabbage");
        break;
    case CANNOT_PUT:
        strcpy(msg.msg_text, "cannot put");
        break;
    case CANNOT_TAKE:
        strcpy(msg.msg_text, "cannot take");
        break;
    case INVALID_COMMAND:
        strcpy(msg.msg_text, "invalid command");
        break;
    default:
        break;
    }
    return msgsnd(msgid, &msg, strlen(msg.msg_text), 0);
}

int main() {
    key_t key = ftok("queue", 1234);
    int msgid = msgget(key, 0666 | IPC_CREAT|IPC_EXCL);
    if (msgid == -1) {
        return 1;
    }

    struct message msg;
    long cur_client_pid; //pid * 2

    int buf_len = 10;
    struct cur_situation* sit_buf = malloc(sizeof(struct cur_situation) * buf_len);
    long* pid_buf = calloc(buf_len, sizeof(long));
    char* busy_buf = calloc(buf_len, sizeof(char));
    int i = 0;

    if ((!sit_buf) || (!pid_buf) || (!busy_buf)) {
        free(sit_buf);
        free(pid_buf);
        free(busy_buf);
        msgctl(msgid, IPC_RMID, NULL);
        return 1;
    }

    while (1) {
        msgrcv(msgid, &msg, sizeof(msg), 0, 0);

        if (msg.msg_type & 1 == 1) { //it is server message
            msgsnd(msgid, &msg, strlen(msg.msg_text), 0); //send back
            continue;
        }

        cur_client_pid = msg.msg_type;

        if(!msg.msg_text[0]) {
            continue;
        }

        if (!strcmp(msg.msg_text, "kill")) {
            break;
        }

        if (!strcmp(msg.msg_text, "new")) {
            i = 0;
            while ((busy_buf[i] != 0) && (i < buf_len)) {
                i++;
            }
            if (i >= buf_len) {
                buf_len <<= 1;
                sit_buf = realloc(sit_buf, buf_len);
                pid_buf = realloc(pid_buf, buf_len);
                busy_buf = realloc(busy_buf, buf_len);
                if ((!sit_buf) || (!pid_buf) || (!busy_buf)) {
                    free(sit_buf);
                    free(pid_buf);
                    free(busy_buf);
                    msgctl(msgid, IPC_RMID, NULL);
                    return 1;
                }
            }
            busy_buf[i] = 1;
            pid_buf[i] = cur_client_pid;
            init_situation(&(sit_buf[i]));
        } else if (!strcmp(msg.msg_text, "end")) {
            i = 0;
            while ((pid_buf[i] != cur_client_pid) && (i < buf_len)) {
                i++;
            }
            if (i < buf_len) {
                pid_buf[i] = 0;
                busy_buf[i] = 0;
            }
            continue;
        } else {
            i = 0;
            while ((pid_buf[i] != cur_client_pid) && (i < buf_len)) {
                i++;
            }
            if (i >= buf_len) {
                continue;
            }
        }

        Result res = process_command(&(sit_buf[i]), msg.msg_text);

        if (res == INVALID_PTR) {
            init_situation(&(sit_buf[i]));
            continue;
        }

        send_answer(msgid, cur_client_pid, res);
    }

    msgctl(msgid, IPC_RMID, NULL);
}
