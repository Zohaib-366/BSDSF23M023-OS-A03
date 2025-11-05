#include "shell.h"
#include <fcntl.h>   // For open()
#include <unistd.h>  // For pipe(), dup2(), close()
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int execute(char* arglist[]) {
    int status;
    int cpid;

    // 1️⃣ Check for pipe '|'
    int pipe_index = -1;
    for (int i = 0; arglist[i] != NULL; i++) {
        if (strcmp(arglist[i], "|") == 0) {
            pipe_index = i;
            break;
        }
    }

    if (pipe_index != -1) {
        // PIPE HANDLING
        arglist[pipe_index] = NULL; // Split the command
        char** left_cmd = arglist;
        char** right_cmd = &arglist[pipe_index + 1];

        int fd[2];
        if (pipe(fd) == -1) {
            perror("pipe failed");
            return -1;
        }

        int cpid1 = fork();
        if (cpid1 == 0) { // Left child (writer)
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]); close(fd[1]);

            // Handle any redirection in left command
            for (int i = 0; left_cmd[i] != NULL; i++) {
                if (strcmp(left_cmd[i], ">") == 0) {
                    char* outfile = left_cmd[i + 1];
                    left_cmd[i] = NULL;
                    int fd_out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd_out < 0) { perror("open"); exit(1); }
                    dup2(fd_out, STDOUT_FILENO);
                    close(fd_out);
                    i++;
                } else if (strcmp(left_cmd[i], "<") == 0) {
                    char* infile = left_cmd[i + 1];
                    left_cmd[i] = NULL;
                    int fd_in = open(infile, O_RDONLY);
                    if (fd_in < 0) { perror("open"); exit(1); }
                    dup2(fd_in, STDIN_FILENO);
                    close(fd_in);
                    i++;
                }
            }

            execvp(left_cmd[0], left_cmd);
            perror("Command not found");
            exit(1);
        }

        int cpid2 = fork();
        if (cpid2 == 0) { // Right child (reader)
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]); close(fd[1]);

            // Handle any redirection in right command
            for (int i = 0; right_cmd[i] != NULL; i++) {
                if (strcmp(right_cmd[i], ">") == 0) {
                    char* outfile = right_cmd[i + 1];
                    right_cmd[i] = NULL;
                    int fd_out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd_out < 0) { perror("open"); exit(1); }
                    dup2(fd_out, STDOUT_FILENO);
                    close(fd_out);
                    i++;
                } else if (strcmp(right_cmd[i], "<") == 0) {
                    char* infile = right_cmd[i + 1];
                    right_cmd[i] = NULL;
                    int fd_in = open(infile, O_RDONLY);
                    if (fd_in < 0) { perror("open"); exit(1); }
                    dup2(fd_in, STDIN_FILENO);
                    close(fd_in);
                    i++;
                }
            }

            execvp(right_cmd[0], right_cmd);
            perror("Command not found");
            exit(1);
        }

        close(fd[0]); close(fd[1]);
        waitpid(cpid1, &status, 0);
        waitpid(cpid2, &status, 0);
        return 0;
    }

    // 2️⃣ No pipe, single command with possible redirection
    cpid = fork();
    switch (cpid) {
        case -1:
            perror("fork failed");
            exit(1);
        case 0: { // CHILD PROCESS
            for (int i = 0; arglist[i] != NULL; i++) {
                if (strcmp(arglist[i], ">") == 0) {
                    char* outfile = arglist[i + 1];
                    arglist[i] = NULL;
                    int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) { perror("open"); exit(1); }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    i++;
                } else if (strcmp(arglist[i], "<") == 0) {
                    char* infile = arglist[i + 1];
                    arglist[i] = NULL;
                    int fd = open(infile, O_RDONLY);
                    if (fd < 0) { perror("open"); exit(1); }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                    i++;
                }
            }

            execvp(arglist[0], arglist);
            perror("Command not found");
            exit(1);
        }
        default: // PARENT PROCESS
            waitpid(cpid, &status, 0);
            return 0;
    }
}

