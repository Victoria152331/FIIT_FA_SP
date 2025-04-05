#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MSG_SIZE 128

struct message {
    long msg_type;
    char msg_text[MSG_SIZE];
};

int main() {
    key_t key = ftok("queue", 1234);
    int msgid = msgget(key, 0666);

    struct message msg;
    msg.msg_type = 4;
    strcpy(msg.msg_text, "kill");
    msgsnd(msgid, &msg, 5, 0);
    //msgctl(msgid, IPC_RMID, NULL);
    return 0;
}