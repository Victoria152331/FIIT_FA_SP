#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 128

enum node_type {
    DIR_NODE, FILE_NODE
};

typedef struct tree_node {
    char* name;
    enum node_type type;
    struct tree_node* bro;
    struct tree_node* son;
} tree_node;

enum add_result {
    SUCCESS, ERROR
};

struct message {
    long msg_type; // тип сообщения
    char msg_text[MSG_SIZE]; // текст сообщения
};

void print_tree(tree_node* node, int depth) {
    if (!node) {
        return;
    }
    for (int i = 0; i < depth; i++) {
        printf("   ");
    }
    printf("%s\n", node->name);
    if (node->son) {
        print_tree(node->son, depth + 1);
    }
    if (node->bro) {
        print_tree(node->bro, depth);
    }
}

enum add_result add_file(tree_node*** roots, tree_node* path, int* roots_len) {
    tree_node* cur_dir = NULL;
    int i;
    for(i = 0; (*roots)[i] != NULL; i++) {
        if (!strcmp(path->name, (*roots)[i]->name)) {
            cur_dir = (*roots)[i];
            break;
        }
    }

    if(cur_dir == NULL) {
        if (i >= *roots_len) {
            *roots_len <<= 2;
            *roots = realloc(*roots, *roots_len);
            if (!(*roots)) {
                return ERROR;
            }
        }
        (*roots)[i] = path;
        (*roots)[i + 1] = NULL; 
        return SUCCESS;
    }

    int flag_found = 0;
    tree_node* buf;
    buf = path;
    path = path->son;
    free(buf->name);
    free(buf);
    while(path->type == DIR_NODE) {
        if (cur_dir->son == NULL) {
            cur_dir->son = path;
            return SUCCESS;
        }
        if ((cur_dir->son)->type != DIR_NODE) {
            path->bro = cur_dir->son;
            cur_dir->son = path;
            return SUCCESS;
        }
        cur_dir = cur_dir->son;
        while (1) {
            if (strcmp(cur_dir->name, path->name)) {
                if (cur_dir->bro == NULL) {
                    cur_dir->bro = path;
                    return SUCCESS;
                } else if ((cur_dir->bro)->type != DIR_NODE) {
                    path->bro = cur_dir->bro;
                    cur_dir->bro = path;
                    return SUCCESS;
                }
                cur_dir = cur_dir->bro;
            } else {
                break;
            }
        }
        buf = path;
        path = path->son;
        free(buf->name);
        free(buf);
    }
    
    if (cur_dir->son == NULL) {
        cur_dir->son = path;
        return SUCCESS;
    }

    cur_dir = cur_dir->son;
    while (1) {
        if (!strcmp(cur_dir->name, path->name)) {
            free(path->name);
            free(path);
            return ERROR;
        }
        if (cur_dir->bro == NULL) {
            cur_dir->bro = path;
            return SUCCESS;
        }
        cur_dir = cur_dir->bro;
    }
}

tree_node* parse_path(const char* str_path) {
    int i = strlen(str_path) - 1;
    int end_name;
    char* cur_name;
    tree_node *path_node = NULL, *new_node = NULL;
    enum node_type type = FILE_NODE;

    while(i >= 0) {
        end_name = i;
        while((i >= 0) && (str_path[i] != '/')) {
            i--;
        }

        cur_name = malloc(sizeof(char) * (end_name - i + 1));
        cur_name[end_name - i] = 0;
        strncpy(cur_name, &(str_path[i + 1]), end_name - i);

        new_node = malloc(sizeof(tree_node));
        new_node->name = cur_name;
        new_node->type = type;
        new_node->bro = NULL;
        new_node->son = path_node;
        path_node = new_node;
        type = DIR_NODE;
        i--;
    }

    return path_node;
}

void del_tree(tree_node* node) {
    if (node == NULL) {
        return;
    }
    del_tree(node->son);
    del_tree(node->bro);
    free(node->name);
    free(node);
}

void print_catalogies_r(tree_node* node, char* path, int msgid, long type) {
    if (node == NULL) {
        return;
    }

    struct message msg;
    msg.msg_type = type;


    size_t current_length = strlen(path);
    if (current_length > 0) {
        strcat(path, "/");
    }
    strcat(path, node->name);
    
    sprintf(msg.msg_text, "Dir: %s\n", path);
    msgsnd(msgid, &msg, sizeof(msg), 0);
    
    tree_node* file_node = node->son;

    while ((file_node != NULL) && (file_node->type == DIR_NODE)) {
        file_node = file_node->bro;
    }

    int k = 0;
    while (file_node != NULL) {
        sprintf(msg.msg_text, "   %s\n", file_node->name);
        msgsnd(msgid, &msg, sizeof(msg), 0);
        k++;
        file_node = file_node->bro;
    }
    if (k == 0) {
        sprintf(msg.msg_text, "   no files\n");
        msgsnd(msgid, &msg, sizeof(msg), 0);
    }

    tree_node* child_node = node->son;
    while ((child_node != NULL) && (child_node->type == DIR_NODE)) {
        print_catalogies_r(child_node, path, msgid, type);
        child_node = child_node->bro;
    }

    path[current_length] = '\0';
}

void print_catalogies(tree_node* root, int msgid, long type) {
    if (root == NULL) {
        return;
    }

    char* path = (char*)malloc(MSG_SIZE * sizeof(char));
    if (path == NULL) {
        return;
    }
    path[0] = 0;

    print_catalogies_r(root, path, msgid, type);
    free(path);
}



int main() {
    key_t key = ftok("queue", 123); // создаем ключ
    int msgid = msgget(key, 0666 | IPC_CREAT); // создаем очередь сообщений

    struct message msg;
    long cur_client_pid;

    tree_node* path;
    tree_node** roots = malloc(sizeof(tree_node*) * 4);
    int roots_len = 4;

    while (1) {
        msgrcv(msgid, &msg, sizeof(msg), 0, 0);
        cur_client_pid = msg.msg_type;

        if(!msg.msg_text[0]) {
            continue;
        }

        printf("receive: %s\n", msg.msg_text);

        path = parse_path(msg.msg_text);
        add_file(&roots, path, &roots_len);

        while (1) {
            msgrcv(msgid, &msg, sizeof(msg), cur_client_pid, 0);
            printf("receive: %s\n", msg.msg_text);
            if (msg.msg_text[0] != 0) {
                path = parse_path(msg.msg_text);
                add_file(&roots, path, &roots_len);
            } else {
                break;
            }
        }

        for(int i = 0; roots[i] != NULL; i++) {
            print_catalogies(roots[i], msgid, cur_client_pid + 1);
        }
        msg.msg_text[0] = 0;
        msg.msg_type = cur_client_pid + 1;
        msgsnd(msgid, &msg, sizeof(msg), 0);
        msgrcv(msgid, &msg, sizeof(msg), cur_client_pid, 0);
        for(int i = 0; roots[i] != NULL; i++) {
            del_tree(roots[i]);
            roots[i] = NULL;
        }
        printf("ready for new client\n");
    }

    msgctl(msgid, IPC_RMID, NULL);
}
