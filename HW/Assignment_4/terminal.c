#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define TERMINAL_PROMPT "csci297 > "
const char TERMINAL_PROMPT_COLOR[] = "\033[0;32m";
const char TERMINAL_COLOR_DEFAULT[] = "\033[0m";
const int MAX_SLEEP = 1000;

#define COMMAND_MAX_CHAR_SIZE 8191
char *PATH[] = {"/usr/bin/", "/bin/", "./", ""};

// DYNAMIC STRING ARRAY TO HOLD TOKENS.
typedef struct
{
  char **array;
  int count;
  int size;
} DynamicArray;

void initialize_array(DynamicArray *arr, int size)
{
  arr->size = size;
  arr->count = 0;
  arr->array = (char **)malloc(size * sizeof(char *));
  if (arr->array == NULL)
  {
    fprintf(stderr, "Failed to Allocated Memory for Dynamic Array\n");
  }
  return;
}

void resize_array(DynamicArray *arr, int size)
{

  arr->size += size;

  char **temp = (char **)realloc(arr->array, arr->size * sizeof(char **));
  if (temp == NULL)
  {
    fprintf(stderr, "Error ! Could not Resize Array.");
    exit(EXIT_SUCCESS);
  }
  arr->array = temp;
  return;
}

void add_to_array(DynamicArray *arr, char *item)
{
  if (arr->count == arr->size)
  {
    resize_array(arr, arr->size + 10);
  }
  arr->array[arr->count] = strdup(item);
  if (arr->array[arr->count] == NULL)
  {
    fprintf(stderr, "Memory Allocation Failed");
  }
  arr->count++;
  return;
}

void print_array(DynamicArray *arr)
{
  printf("\n[");
  for (int i = 0; i < arr->count; i++)
  {
    printf("%s,", arr->array[i]);
  }
  printf("]\n");
}

// A STRUCT FOR COMMAND WITH ALL ITS PARAMETERS:
typedef struct
{
  char *command_full_path;
  char *args;
  char *in;
  char *out;
} CommandInstruction;

void reset_current_command(CommandInstruction *ptr)
{
  ptr->command_full_path = NULL;
  initialize_array(ptr->args, 1);
  ptr->in = "";
  ptr->out = "";
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

DynamicArray tokenizer(char command[])
{
  size_t len = strlen(command);
  DynamicArray tokens;
  initialize_array(&tokens, 10);
  // Using strtok to break down string into tokens. Did not hardcode this even though it would be tedious rather than difficult.
  char *token = strtok(command, " ");
  while (token != NULL)
  {
    // printf("Token: %s, Size: %lu\n, Last Characer: '%d'", token, strlen(token), token[strlen(token) - 1]);
    if (token[strlen(token) - 1] == '\n')
    {
      token[strlen(token) - 1] = '\0';
    }
    add_to_array(&tokens, token);
    token = strtok(NULL, " ");
  }
  return tokens;
}

void do_command(CommandInstruction *current_command)
{
  pid_t pid;
  printf("\nCommand: %s; With Args: %d : StdIn: %s | Stdout: %s\n", current_command->command_full_path, current_command->args->count - 1, current_command->in, current_command->out);
  print_array(current_command->args);
  if ((pid = fork()) == 0)
  {
    if (execve(current_command->command_full_path, current_command->args->array, PATH) < 0)
    {
      printf("%s: Command was not found ! \n", current_command->command_full_path);
      exit(0);
    }
  }
  else
  {
    printf("%s Ran With PID[%d]\n", current_command->command_full_path, pid);
    int status;
    if (waitpid(pid, &status, 0) < 0)
    {
    }
  }
  return;
}

void built_in_commands(char *cmd)
{

  if (strcmp(cmd, "exit") == 0)
  {
    printf("\nExiting Terminal Session (PID = %d)", getppid());
    exit(EXIT_SUCCESS);
  }
  return;
}

void parse(char command[])
{
  DynamicArray tokens = tokenizer(command);
  built_in_commands(tokens.array[0]);
  CommandInstruction current_command;
  reset_current_command(&current_command);
  for (int i = 0; i < tokens.count; i++)
  {
    printf("\nCurrent Token: %s\n", tokens.array[i]);
    if (current_command.command_full_path == NULL)
    {
      current_command.command_full_path = get_bin_path(tokens.array[i]);
      if (!strcmp(current_command.command_full_path, ""))
      {
        printf("\nError PATH Not Found For %s \n", tokens.array[i]);
        break;
      }
    }
    // i++ to skip the next token since the next token has to be the filename.
    else if (strcmp(tokens.array[i], ">") == 0)
    {
      i++;
      if (i >= tokens.count || strcmp(tokens.array[i], ";") == 0)
      {
        printf("Error, IO Redirection Failed: FileName Not Found.\n");
        return;
      }
      current_command.out = tokens.array[i];
    }
    else if (strcmp(tokens.array[i], "<") == 0)
    {
      i++;
      if (i >= tokens.count)
      {
        printf("Error, IO Redirection Failed: FileName Not Found.\n");
        return;
      }
      current_command.in = tokens.array[i];
    }
    else if (strcmp(tokens.array[i], ";") == 0)
    {
      do_command(&current_command);
    }
    else
    {
      add_to_array(current_command.args, tokens.array[i]);
    }
  }
  if (strcmp(current_command.command_full_path, "") != 0)
  {
    do_command(&current_command);
  }
  free(tokens.array);
  return;
}

// Using write instead of printf since printf is not async-signal-safe.
void print_prompt()
{
  write(STDOUT_FILENO, "\n", sizeof(char));
  write(STDOUT_FILENO, TERMINAL_PROMPT_COLOR, sizeof(TERMINAL_PROMPT_COLOR) - 1);
  write(STDOUT_FILENO, TERMINAL_PROMPT, sizeof(TERMINAL_PROMPT) - 1);
  write(STDOUT_FILENO, TERMINAL_COLOR_DEFAULT, sizeof(TERMINAL_COLOR_DEFAULT) - 1);
}

// Signal Handler.
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

int main()
{
  char command[COMMAND_MAX_CHAR_SIZE];
  // Signal Handlers:
  // signal(SIGINT, sig_handler);
  // signal(SIGTSTP, sig_handler);
  while (1)
  {
    // Gets the Command. Makes the Prompt Green/Or Set Color
    print_prompt();
    fgets(command, COMMAND_MAX_CHAR_SIZE, stdin);
    // Command is stored in char array and passed into parse.
    parse(command);
  }
}
