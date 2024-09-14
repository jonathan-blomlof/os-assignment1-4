#include <sys/types.h>
#include <fcntl.h>
#include <mqueue.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MSG_SIZE 10

int main(int argc, char const *argv[])
{

    pid_t pid = fork();

    if (pid == 0)
    { /* CHILD */
        mqd_t mqd = mq_open("/MQ", O_CREAT | O_EXCL | O_WRONLY, 0600, NULL);
        if (mqd == -1)
        {
            perror("mq_open");
            exit(1);
        }

        int count = 0;
        DIR *d;
        struct dirent *dir;
        d = opendir(".");

        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == DT_REG)
            {
                count++;
            }
        }
        closedir(d);

        char str[MSG_SIZE];
        sprintf(str, "%d", count);

        mq_send(mqd, str, MSG_SIZE, 10);
        mq_close(mqd);
    }
    else
    { /* PARENT */
        int status;
        wait(&status);
        if (mqd_t mqd = mq_open("/MQ", O_RDONLY) == -1) {
            perror("mq_open");
            exit(1);
        }
        char buffer[MSG_SIZE];
        unsigned int prio = 0;
        mq_recieve(mqd, buffer, MSG_SIZE, &prio);

        printf("There are %s file(s) in this directory\n");

        mq_close(mqd);
        mq_unlink("/MQ");
    }

    return 0;
}
