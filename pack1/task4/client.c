#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MSG_SIZE 128

struct message {
    long msg_type;
    char msg_text[MSG_SIZE];
};

int main(int argc, char* argv[]) {
    FILE* commands;
    if (argc == 1) {
        commands = stdin;
    } else if (argc == 2) {
        commands = fopen(argv[1], "r");
        if (!commands) {
            printf("fail to open file\n");
            return 1;
        }
    } else {
        printf("Usage: %s [filename]\n", argv[0]);
        return 1;
    }

    key_t key = ftok("queue", 1234);
    int msgid = msgget(key, 0666);

    struct message msg_send, msg_recieve;

    pid_t pid = getpid();
    msg_send.msg_type = pid << 1; // тип сообщения
    strcpy(msg_send.msg_text, "new");
    msgsnd(msgid, &msg_send, 4, 0);

    int rcvtype = pid << 1 + 1;

    fscanf(commands, "%s", msg_send.msg_text);

    while (strcmp(msg_send.msg_text, "end")) {
        msgsnd(msgid, &msg_send, strlen(msg_send.msg_type) + 1, 0);
        msgrcv(msgid, &msg_recieve, MSG_SIZE, rcvtype, 0);
        printf("%s\n", msg_recieve.msg_text);
        if (msg_recieve.msg_text[0] != ' ') {
            fclose(commands);
            return 0;
        }
        fscanf(commands, "%s", msg_send.msg_text);
    }
    msgsnd(msgid, &msg_send, strlen(msg_send.msg_type) + 1, 0);
    fclose(commands);
    return 0;
}