// test.c
// Tawfeeq Mannan

#include <string.h>     // strcmp
#include <stdio.h>      // printf, scanf
#include <stdlib.h>     // atoi
#include <unistd.h>     // getpid, sleep
#include <sys/types.h>  // pid_t


int main(int argc, char **argv)
{
    pid_t pid = getpid();
    printf("Program %s is executing with PID %d.\n", argv[0], pid);

    printf("Arguments received:\n");
    for (int i = 0; i < argc; i++)
        printf("    %s\n", argv[i]);

    int duration = 1;
    if (argc >= 2)
    {
        if (strcmp(argv[1], "auto") == 0)
        {
            if (argc >= 3)
                duration = atoi(argv[2]);
        }
        else if (strcmp(argv[1], "input") == 0)
        {
            printf("Enter number of seconds to sleep: ");
            scanf("%d", &duration);
        }
        else
        {
            duration = 3;
        }
    }

    sleep(duration);
    printf("All done! Exiting %s now.\n", argv[0]);

    return 0;
}
