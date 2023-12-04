#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

// A STRUCT FOR COMMAND WITH ALL ITS PARAMETERS:
typedef struct
{
    char *command;
    char **args;
    char *in;
    char *out;
    int args_count;
} CommandInstruction;

// Shell Constants:
#define TERMINAL_PROMPT "csci297 > "
const char TERMINAL_PROMPT_COLOR[] = "\033[0;32m";
const char TERMINAL_COLOR_DEFAULT[] = "\033[0m";
const int MAXARGS = 128;
const int MAXLINE = 384;

// PATHS
char *PATH[] = {"/usr/bin/", "/bin/", "./", ""};

// Fork Wrapper:
pid_t Fork(void)
{
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        fprintf(stderr, "Fork Error: \n");
        exit(0);
    }
    return pid;
}

// An Async-Safe Print Prompt Function
void print_prompt()
{
    write(STDOUT_FILENO, "\n", sizeof(char));
    write(STDOUT_FILENO, TERMINAL_PROMPT_COLOR, sizeof(TERMINAL_PROMPT_COLOR) - 1);
    write(STDOUT_FILENO, TERMINAL_PROMPT, sizeof(TERMINAL_PROMPT) - 1);
    write(STDOUT_FILENO, TERMINAL_COLOR_DEFAULT, sizeof(TERMINAL_COLOR_DEFAULT) - 1);
}

void sig_handler(int sig_num)
{
    write(STDOUT_FILENO, "\n", sizeof(char));
    if (sig_num == SIGINT)
    {
        write(STDOUT_FILENO, "caught sigint\n", sizeof("caught sigint") - 1);
    }
    else if (sig_num == SIGTSTP)
    {

        write(STDOUT_FILENO, "caught sigstp\n", sizeof("caught sigstp") - 1);
    }
    print_prompt();
}

int file_exists(char *file_name)
{
    return access(file_name, F_OK);
}

char *get_bin_path(char *executable_name)
{
    for (int i = 0; i < sizeof(PATH) / sizeof(char *); i++)
    {
        size_t full_path_size = strlen(PATH[i]) + strlen(executable_name) + 1;
        // Allocate memory for full_path dynamically
        char *full_path = malloc(full_path_size);
        full_path[0]='\0';
        // Check if memory allocation is successful
        if (full_path == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        // Concatenate PATH and Executable Name
        strcat(full_path, PATH[i]);
        strcat(full_path, executable_name);

        if (file_exists(full_path) == 0)
        {
            return full_path;
        }
        free(full_path);
    }
    return "";
}

void print_string_with_index(char *str)
{
    char buf;
    int i = 0;
    while ((buf = str[i]) != '\0')
    {
        printf("Indx:%d | Char: %c ", i, buf);
        i++;
    }
    printf("\n");
    return;
}

// Basically a Tokenizer. Will Separate ";" into a separate index as an arg.
int parseline(char *buf, char **argv)
{
    char *delim;
    int argc;
    int bg;
    buf[strlen(buf) - 1] = ' ';
    while (*buf && (*buf == ' '))
    {
        buf++;
    }
    argc = 0;
    while ((delim = strchr(buf, ' ')))
    {
        argv[argc++] = buf;
        *delim = '\0';
        // detect ';' and strip them from command if suffix
        // and then push it as an item in the argv
        if (strlen(buf) != 1 && buf[strlen(buf) - 1] == ';')
        {
            buf[strlen(buf) - 1] = '\0';
            argv[argc++] = ";";
        }
        buf = delim + 1;
        while (*buf && (*buf == ' '))
        {
            buf++;
        }
    }
    argv[argc] = NULL;
    if (argc == 0)
    {
        return 1;
    }

    if ((bg = (*argv[argc - 1] == '&')) != 0)
    {
        argv[--argc] = NULL;
    }
    return bg;
}

int builtin_command(char *cmd)
{
    if (!strcmp(cmd, "exit"))
    {
        printf("Exiting Terminal. \n");
        exit(0);
    }
    if (!strcmp(cmd, "&"))
    {
        return 1;
    }
    return 0;
}

CommandInstruction instantiate_command(int arg_size)
{
    CommandInstruction command;
    command.command = NULL;
    command.in = NULL;
    command.out = NULL;
    command.args = malloc(arg_size * sizeof(char *));
    command.args_count = 0;
    return command;
}

void command_runner(CommandInstruction *command)
{
    if (command->command == NULL)
    {
        return;
    }
    int pid;
    int status; 
    if (!builtin_command(command->command))
    {
        char *bin_path = get_bin_path(command->command);
        if ((pid = Fork()) == 0)
        {
            if (command->out)
            {
                int fileDescriptor = open(command->out, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fileDescriptor == -1)
                {
                    perror("Error opening file");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fileDescriptor, STDOUT_FILENO) == -1)
                {
                    perror("Error redirecting stdout");
                    exit(EXIT_FAILURE);
                }
                close(fileDescriptor);
            }
            if (command->in)
            {
                int fileDescriptor = open(command->in, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fileDescriptor == -1)
                {
                    perror("Error opening file");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fileDescriptor, STDIN_FILENO) == -1)
                {
                    perror("Error redirecting stdout");
                    exit(EXIT_FAILURE);
                }
                close(fileDescriptor);
            }
            if (execvp(bin_path, command->args) < 0)
            {
                printf("%s: Command was not found.\n", command->command);
                exit(0);
            }
        }
        do{
            waitpid(pid, &status, WUNTRACED);

        }while(!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));

        printf("[PID: %d] Running %s\n", pid, command->command);
        if (strlen(bin_path) > 0)
        {
            free(bin_path);
        }
    }
    return;
}

void token_analyser(char **argv)
{
    int i = 0;
    int command_indx = 0;
    CommandInstruction current_command = instantiate_command(MAXARGS - i);
    while (1)
    {
        if (argv[i] == NULL)
        {
            break;
        }
        if (argv[i] != NULL && current_command.command == NULL)
        {
            current_command.command = argv[i];
        }
        else if (strcmp(argv[i], "<") == 0)
        {
            i++;
            current_command.in = argv[i];
        }
        else if (strcmp(argv[i], ">") == 0)
        {
            i++;
            current_command.out = argv[i];
        }
        else if (strcmp(argv[i], ";") == 0)
        {
            command_runner(&current_command);
            current_command = instantiate_command(MAXARGS - i);
        }
        else
    {
            current_command.args[current_command.args_count] = argv[i];
            current_command.args_count++;
        }
        i++;
    }
    command_runner(&current_command);
    return;
}

// CMD Evaluator
void eval(char *cmdline)
{
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int bg;
    pid_t pid;
    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL)
    {
        return;
    }
    token_analyser(argv);
};

int main()
{
    char cmdline[MAXLINE];
    // Signal Handlers:
    signal(SIGINT, sig_handler);
    signal(SIGTSTP, sig_handler);

    while (1)
    {
        print_prompt();
        fgets(cmdline, MAXLINE, stdin);
        if (feof(stdin))
        {
            return (0);
        }
        eval(cmdline);
    }
};
