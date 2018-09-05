#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <semaphore.h>

#include "fcmd.h"

int main(int argc, char *argv[])
{
	char *input, shell_prompt[100];

    // Configure readline to auto-complete paths when the tab key is hit.
    rl_bind_key('\t', rl_complete);

    for (;;)
    {
        // Create prompt string from user name and current working directory.
        snprintf(shell_prompt, sizeof(shell_prompt), "%s $ ", getenv("USER"));
  
        // Display prompt and read input (n.b. input must be freed after use)...
        input = readline(shell_prompt);
  
        // Check for EOF.
        if (!input)
            break;
  
        // Add input to history.
        add_history(input);
		
        // Do stuff...
		fcmd_exec(input);
  
        // Free input.
        free(input);
		input = NULL;
    }

    return 0;
}
