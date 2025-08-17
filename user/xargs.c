#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define XBUFFER 512

int main(int argc, char *argv[]) {
    char* args[MAXARG] = {0};
    int argsIndex = 0;
    int maxArgs = -1;

    for (int index = /* skip argv[0] */ 1; index < argc; index += 1) {
        if (strcmp(argv[index], "-n") == 0) {
            maxArgs = atoi(argv[index + 1]);
            index += 1;
            continue;
        }

        if (argsIndex >= MAXARG) {
            fprintf(2, "xargs: too many arguments (main)\n");
            exit(1);
        }

        args[argsIndex] = argv[index];
        argsIndex += 1;
    }

    while (1) {
        int argsIndexAll = argsIndex;
        for (int index = argsIndexAll; index < MAXARG; index += 1) {
            args[index] = 0;
        }

        int read_n = 0;
        char buffer[XBUFFER] = {0};
        for (int bufferIndex = 0;; bufferIndex += 1) {
            if (bufferIndex >= XBUFFER) {
                fprintf(2, "xargs: buffer exceeded\n");
                exit(1);
            }

            read_n = read(0, buffer + bufferIndex, 1);
            if (read_n == 0 /* EOF */ || buffer[bufferIndex] == '\n') {
                buffer[bufferIndex] = '\0';
                break;
            }

            if (buffer[bufferIndex] == ' ') {
                buffer[bufferIndex] = '\0';

                if (argsIndexAll - argsIndex == maxArgs) {
                    break;
                }

                continue;
            }

            if (bufferIndex == 0 || buffer[bufferIndex - 1] == '\0') {
                if (argsIndexAll >= MAXARG) {
                    fprintf(2, "xargs: too many arguments (extended)\n");
                    exit(1);
                }

                args[argsIndexAll] = buffer + bufferIndex;
                argsIndexAll += 1;
            }
        }

        if (fork() == 0) {
            exec(args[0], args);
            fprintf(2, "xargs: execution failed\n");
            exit(1);
        } else {
            wait(0);

            if (read_n == 0) {
                break;
            }
        }
    }

    exit(0);
}
