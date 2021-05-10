#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "parse.h"
#include "cmd.h"
#include "def.h"

typedef enum {
    TOKEN_SYM, TOKEN_STR, TOKEN_PIPE,
    TOKEN_ANGLE_L, TOKEN_ANGLE_R, TOKEN_ANGLE_RR,
    TOKEN_END
} TokenType;

typedef struct {
    TokenType token_type;
    char *str; // should not free
    size_t len;
} Token;

void add_token(Token *tokens, TokenType token_type, char *str) {
    tokens->token_type = token_type;
    tokens->str = str;
    tokens->len = token_type == TOKEN_ANGLE_RR ? 2 : 1;
}

bool tokenize(char *buf, Token *tokens) {
    while (*buf) {
        while (*buf && isspace(*buf)) ++buf;
        if (!*buf) break;

        if (*buf == '|') add_token(tokens, TOKEN_PIPE, buf), ++buf;
        else if (*buf == '>' || *buf == '<') {
            TokenType token_type = *buf == '<' ? TOKEN_ANGLE_L :
                                   *buf == '>' && *(buf + 1) == '>' ? TOKEN_ANGLE_RR :
                                   TOKEN_ANGLE_R;
            ++buf;
            add_token(tokens, token_type, NULL);
        } else if (*buf == '\'' || *buf == '\"') {
            tokens->token_type = TOKEN_STR;
            tokens->str = buf;

            char *start = buf;
            ++buf; // ' / "
            while (*buf && *buf != *start) buf += (*(buf + 1) && *buf == '\\') ? 2 : 1;
            ++buf; // ' / "
            tokens->len = buf - start;
        } else { // symbol
            tokens->token_type = TOKEN_SYM;
            tokens->str = buf;

            char *start = buf;
            while (*buf && !isspace(*buf) &&
                   *buf != '|' && *buf != '<' && *buf != '>')
                ++buf;
            tokens->len = buf - start;
        }

        ++tokens;
    }
    add_token(tokens, TOKEN_END, NULL);

    return true;
}

size_t count_args(Token *tokens) {
    Token *start = tokens;
    while (tokens->token_type != TOKEN_END && tokens->token_type != TOKEN_PIPE) ++tokens;
    return tokens - start + 1;
}

#define alloc_and_move_token_str(property) \
    do {                       \
        (property) = malloc(sizeof(char) * (tokens->len + 1)); \
        memcpy(property, tokens->str, sizeof(char) * tokens->len); \
        (property)[tokens->len] = '\0'; \
        } while(0)

bool parse(Token *tokens, Command *commands) {
    while (tokens->token_type != TOKEN_END) {
        if (tokens->token_type != TOKEN_SYM) return false;

        command_init(commands);
        char **args = commands->args = malloc(sizeof(char *) * count_args(tokens));
        alloc_and_move_token_str(commands->cmd);
        alloc_and_move_token_str(*args);
        ++args;
        ++tokens;

        while (tokens->token_type != TOKEN_END && tokens->token_type != TOKEN_PIPE) {
            if (tokens->token_type == TOKEN_SYM || tokens->token_type == TOKEN_STR) {
                alloc_and_move_token_str(*args);
                ++tokens;
                args++;
            } else if (tokens->token_type == TOKEN_ANGLE_L ||
                       tokens->token_type == TOKEN_ANGLE_R ||
                       tokens->token_type == TOKEN_ANGLE_RR) {
                char **dstp = tokens->token_type == TOKEN_ANGLE_L ? &commands->file_in :
                              tokens->token_type == TOKEN_ANGLE_R ? &commands->file_out :
                              &commands->file_append;
                ++tokens;
                alloc_and_move_token_str(*dstp);
                ++tokens;
            }
        }

        *args = malloc(sizeof(char));
        *args = 0;
        ++commands;
        if (tokens->token_type == TOKEN_PIPE) ++tokens;
    }
    commands->cmd = NULL;
    return true;
}

#undef alloc_and_move_token_str

#define ret_and_free(ret) do { free(tokens); return ret; } while(0)

bool parse_commands(char *buf, Command *commands) {
    bool r = false;
    Token *tokens = malloc(TOKENS_SIZE * sizeof(Token));
    if (tokens == NULL) return false;

    r = tokenize(buf, tokens);
    if (r == false) ret_and_free(false);

    r = parse(tokens, commands);
    if (r == false) ret_and_free(false);

    ret_and_free(true);
}

#undef ret_and_free
