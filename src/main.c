#include "shell.h"

int main() {
    char* cmdline;
    char** arglist;

    while ((cmdline = read_cmd(PROMPT, stdin)) != NULL) {

        int is_history_reexec = 0;

        // -------------------------------
        // Handle !n command (re-execution)
        // -------------------------------
        if (cmdline[0] == '!' && strlen(cmdline) > 1) {
            int n = atoi(cmdline + 1);
            if (n > 0 && n <= history_count) {
                free(cmdline);
                cmdline = strdup(history[n - 1]);
                printf("Re-executing command: %s\n", cmdline);
                is_history_reexec = 1; // mark so we don’t store it again
            } else {
                printf("Invalid history reference: %s\n", cmdline);
                free(cmdline);
                continue; // skip this iteration
            }
        }

        // -------------------------------
        // Store command in history
        // -------------------------------
        if (!is_history_reexec && cmdline && strlen(cmdline) > 0) {
            if (history_count < HISTORY_SIZE) {
                history[history_count] = strdup(cmdline);
            } else {
                free(history[0]);
                for (int i = 1; i < HISTORY_SIZE; i++) {
                    history[i - 1] = history[i];
                }
                history[HISTORY_SIZE - 1] = strdup(cmdline);
            }
            if (history_count < HISTORY_SIZE)
                history_count++;
        }

        // -------------------------------
        // Process command
        // -------------------------------
        if ((arglist = tokenize(cmdline)) != NULL) {

            // Check for built-in commands first
            if (!handle_builtin(arglist)) {
                execute(arglist);
            }

            // Free memory allocated by tokenize()
            for (int i = 0; arglist[i] != NULL; i++) {
                free(arglist[i]);
            }
            free(arglist);
        }

        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}

