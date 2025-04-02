
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MSG_SIZE 128

struct message {
    long msg_type; // тип сообщения
    char msg_text[MSG_SIZE]; // текст сообщения
};

int main() {
    key_t key = ftok("queue", 123); // создаем ключ
    int msgid = msgget(key, 0666); // получаем очередь сообщений

    struct message msg;
    
    msg.msg_text[0] = 'a';
    pid_t pid = getpid();
    msg.msg_type = pid << 1; // тип сообщения
    scanf("%s", msg.msg_text);

    while(strcmp(msg.msg_text, "end")) {
        msgsnd(msgid, &msg, sizeof(msg), 0);
        printf("send: %s\n", msg.msg_text);
        scanf("%s", msg.msg_text);
    }
    printf("all send\n");
    msg.msg_text[0] = 0;
    msgsnd(msgid, &msg, sizeof(msg), 0);

    while (1) {
        msgrcv(msgid, &msg, sizeof(msg), (pid << 1) + 1, 0);
        if (msg.msg_text[0] != 0) {
            printf("%s", msg.msg_text);
        } else {
            break;
        }
    }
    msg.msg_type = pid << 1;
    msgsnd(msgid, &msg, sizeof(msg), 0);
    
    return 0;
}
