#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <semaphore.h>

#include "fcmd/fcmd.h"

int main(int argc, char *argv[])
{
	char *input, shell_prompt[100];

	printf("%s,%s,%s,%s\r\n", BUILD_DATE, BUILD_TIME, PROJECT_NAME, USER_SW_VERSION);

    rl_bind_key('\t', rl_complete);

    for (;;)
    {
        snprintf(shell_prompt, sizeof(shell_prompt), "%s $ ", getenv("USER"));
        input = readline(shell_prompt);
        if (!input)
            break;
  
        add_history(input);
		
        fcmd_exec(input);
        free(input);
		input = NULL;
    }

    return 0;
}
