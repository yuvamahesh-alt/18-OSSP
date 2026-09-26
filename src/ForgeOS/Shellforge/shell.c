#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define INPUT_SIZE 1024
#define MAX_ARGS 64

static void execute_command(char *input)
{
    char *args[MAX_ARGS];
    int argc = 0;

    /* Split input into command + arguments */
    char *token = strtok(input, " \t");

    while (token != NULL && argc < MAX_ARGS - 1)
    {
        args[argc++] = token;
        token = strtok(NULL, " \t");
    }

    args[argc] = NULL;

    /* Empty command */
    if (argc == 0)
        return;

    /* Built-in exit */
    if (strcmp(args[0], "exit") == 0)
    {
        printf("Exiting Shellforge...\n");
        exit(0);
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        /* Child process */
        printf("[Child] PID=%d, PPID=%d\n", getpid(), getppid());

        execvp(args[0], args);

        /* Only reached if execvp fails */
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else
    {
        /* Parent process */
        int status;

        printf("[Parent] PID=%d, Child PID=%d\n",
               getpid(), pid);

        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid");
            return;
        }

        if (WIFEXITED(status))
        {
            printf("[Parent] Child exited with status %d\n",
                   WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("[Parent] Child terminated by signal %d\n",
                   WTERMSIG(status));
        }
    }
}

int main(void)
{
    char input[INPUT_SIZE];

    printf("=====================================\n");
    printf("          ForgeOS Shellforge\n");
    printf("      Unix-Style Shell Project\n");
    printf("=====================================\n");

    while (1)
    {
        printf("ForgeOS> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\nExiting Shellforge...\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        execute_command(input);
    }

    return 0;
}
