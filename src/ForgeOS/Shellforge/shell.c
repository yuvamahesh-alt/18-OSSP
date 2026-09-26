#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define INPUT_SIZE 1024
#define MAX_ARGS 64

/*
 * Command parser for Shellforge.
 *
 * Supports:
 *   - spaces/tabs between arguments
 *   - single quotes: 'hello world'
 *   - double quotes: "hello world"
 *   - backslash escaping: hello\ world
 */

static int parse_command(char *input, char *args[])
{
    int argc = 0;
    char *src = input;

    while (*src != '\0' && argc < MAX_ARGS - 1)
    {
        /* Skip spaces before the next argument */
        while (*src == ' ' || *src == '\t')
        {
            src++;
        }

        if (*src == '\0')
        {
            break;
        }

        char *arg_start = src;
        char *dst = src;

        int in_single_quote = 0;
        int in_double_quote = 0;

        while (*src != '\0')
        {
            /* Single quote */
            if (*src == '\'' && !in_double_quote)
            {
                in_single_quote = !in_single_quote;
                src++;
                continue;
            }

            /* Double quote */
            if (*src == '"' && !in_single_quote)
            {
                in_double_quote = !in_double_quote;
                src++;
                continue;
            }

            /*
             * Backslash escapes the next character.
             * We do not treat backslash specially inside
             * single quotes.
             */
            if (*src == '\\' && !in_single_quote)
            {
                src++;

                if (*src != '\0')
                {
                    *dst++ = *src++;
                }

                continue;
            }

            /*
             * Space/tab ends the argument only when we
             * are outside quotes.
             */
            if ((*src == ' ' || *src == '\t') &&
                !in_single_quote &&
                !in_double_quote)
            {
                /*
                 * Move src past the separator BEFORE
                 * writing the terminating '\0'.
                 */
                src++;
                break;
            }

            *dst++ = *src++;
        }

        if (in_single_quote || in_double_quote)
        {
            fprintf(stderr, "Shellforge: unmatched quote\n");
            return -1;
        }

        *dst = '\0';

        if (*arg_start != '\0')
        {
            args[argc++] = arg_start;
        }
    }

    args[argc] = NULL;

    return argc;
}

static void execute_command(char *input)
{
    char *args[MAX_ARGS];

    int argc = parse_command(input, args);

    if (argc < 0)
    {
        return;
    }

    if (argc == 0)
    {
        return;
    }

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
        printf("[Child] PID=%d PPID=%d\n",
               getpid(), getppid());

        execvp(args[0], args);

        perror("execvp");
        exit(EXIT_FAILURE);
    }

    /* Parent */
    printf("[Parent] PID=%d Child PID=%d\n",
           getpid(), pid);

    int status;

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

