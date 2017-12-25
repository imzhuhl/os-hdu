#include "init.h"

//debug func
void test()
{
    int openfile_num = 0;
    int i;
    printf("debug area ############\n");
    // print open file number
    for (i = 0; i < MAXOPENFILE; i++) {
        if (openfilelist[i].topenfile == 1) {
            openfile_num++;
        }
    }
    printf("  open file number: %d\n", openfile_num);
    printf("  curr file name: %s\n", openfilelist[currfd].filename);
    printf("  curr file length: %d\n", openfilelist[currfd].length);

    printf("debug end  ############\n");
}

int main(void)
{
    char cmd[13][10] = {
        "mkdir", "rmdir", "ls", "cd", "create",
        "rm", "open", "close", "write", "read",
        "exit", "help", "test"
    };
    char command[30], *sp;
    int cmd_idx, i;

    printf("************* file system ***************\n");
    startsys();

    while (1) {
        printf("%s> ", openfilelist[currfd].dir);
        gets(command);
        cmd_idx = -1;
        if (strcmp(command, "") == 0) {
            printf("\n");
            continue;
        }
        sp = strtok(command, " ");
        for (i = 0; i < 13; i++) {
            if (strcmp(sp, cmd[i]) == 0) {
                cmd_idx = i;
                break;
            }
        }
        switch (cmd_idx) {
        case 0: // mkdir
            sp = strtok(NULL, " ");
            if (sp != NULL)
                my_mkdir(sp);
            else
                printf("mkdir error\n");
            break;
        case 1: // rmdir
            sp = strtok(NULL, " ");
            if (sp != NULL)
                my_rmdir(sp);
            else
                printf("rmdir error\n");
            break;
        case 2: // ls
            my_ls();
            break;
        case 3: // cd
            sp = strtok(NULL, " ");
            if (sp != NULL)
                my_cd(sp);
            else
                printf("cd error\n");
            break;
        case 4: // create
            sp = strtok(NULL, " ");
            if (sp != NULL)
                my_create(sp);
            else
                printf("create error\n");
            break;
        case 5: // rm
            sp = strtok(NULL, " ");
            if (sp != NULL)
                my_rm(sp);
            else
                printf("rm error\n");
            break;
        case 6: // open
            sp = strtok(NULL, " ");
            if (sp != NULL)
                my_open(sp);
            else
                printf("open error\n");
            break;
        case 7: // close
            if (openfilelist[currfd].attribute == 1)
                my_close(currfd);
            else
                printf("there is not openning file\n");
            break;
        case 8: // write
            if (openfilelist[currfd].attribute == 1)
                my_write(currfd);
            else
                printf("please open file first, then write\n");
            break;
        case 9: // read
            if (openfilelist[currfd].attribute == 1)
                my_read(currfd);
            else
                printf("please open file first, then read\n");
            break;
        case 10: // exit
            exitsys();
            printf("**************** exit file system ****************\n");
            return 0;
            break;
        case 11: // help
            help();
            break;
        case 12: // test
            test();
            break;
        default:
            printf("wrong command: %s\n", command);
            break;
        }
    }
    return 0;
}
