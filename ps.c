#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

bool abbreviate_home_path(char *path) {
    const char *home = getenv("HOME"); // should not free
    if (home == NULL) return false;

    if (strstr(home, path) == 0) {
        path[0] = '~';

        size_t len_home = strlen(home);
        size_t len_path = strlen(path);
        memmove(path + 1, path + len_home, len_path - len_home);

        path[len_path - len_home + 1] = '\0';
    }
    return true;
}

bool print_ps() {
    char *path = getcwd(NULL, 0);
    if (path == NULL) return false;

    bool r = abbreviate_home_path(path);
    if (r == false) return false;

    printf("\033[0;32m%s\033[0m $ ", path);
    free(path);
    return true;
}

