#ifndef MYSHELL_ERR_H
#define MYSHELL_ERR_H

#define E_SYNTAX do { \
        fprintf(stderr, "Error: Syntax error!\n"); \
        return false; \
    } while (0)

#endif //MYSHELL_ERR_H
