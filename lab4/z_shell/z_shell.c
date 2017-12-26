#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

char *sys_cmd [4] = {"exit", "cmd1", "cmd2", "cmd3"};

int get_cmd_id(char *cmd)
{
    int i;
    for (i = 0; i < 4; i++) {
        if (strcmp(cmd, sys_cmd[i]) == 0) {
            return i;
        }
    }
    return -1;
}

void z_fork(int cmd_id)
{
    pid_t pid = fork();
    if (pid < 0) {
        printf("fork error\n");
        exit(0);
    } else if (pid == 0) {
        // child task
        switch (cmd_id) {
        case 1:
            execl("./cmd1", "", NULL);
            break;
        case 2:
            execl("./cmd2", "", NULL);
            break;
        case 3:
            execl("./cmd3", "", NULL);
            break;
        }
        // error area
        printf("execl error\n");
        exit(0);
    } else {
        return;
    }
}

int main(void)
{
    char cmd[50];
    int cmd_id = -1;
    while (1) {
        printf("command >> ");
        scanf("%s", cmd);
        cmd_id = get_cmd_id(cmd);
        if (cmd_id == -1) {
            printf("command not found\n");
        } else if (cmd_id == 0) {
            exit(0);
        } else {
            z_fork(cmd_id);
        }
        wait(NULL);
    }
}
