#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <wctype.h>
#include <string.h>
#include <wchar.h>

enum {LINE_COUNT, WORD_COUNT, CHAR_COUNT, M_COUNT} mode = LINE_COUNT;


int word_cnt(char *line) {
    int cnt = 0;
    char *token = strtok(line, " \t\n\r");
    while(token != NULL) {
        cnt++;
        printf("%s\n", token);
        token = strtok(NULL, " \t\n\r");
 
    }
    return cnt;
}

size_t m_cnt(char *line) {
    mbstate_t state = {0};
    const char *src = line;

    size_t cnt = mbsrtowcs(NULL, &src, 0, &state);
    return cnt;
}

int open_file(char *file_name, int count_mode) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_count = 0, word_count = 0, char_count = 0, m_count = 0;

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }
    
    while ((read = getline(&line, &len, fp)) != -1) {
        switch(count_mode) {
            case LINE_COUNT:
                line_count++;
                break;
            case  WORD_COUNT:
                word_count += word_cnt(line);
                break;
            case CHAR_COUNT:
                char_count += strlen(line);
                break;
            case M_COUNT:
                m_count += m_cnt(line);
                break;
            default:
                printf("whoops\n");
                exit(EXIT_FAILURE);
        }
    }
    fclose(fp);
    printf("line count: %d, word count %d, char count %d, multibyte count %d \n", line_count, word_count, char_count, m_count);
    if (line)
        free(line);
    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "lwcm")) != -1) {
        switch (opt) {
            case 'l':
                mode = LINE_COUNT;
                break;
            case 'w':
                mode = WORD_COUNT;
                break;
            case 'c':
                mode = CHAR_COUNT;
                break;
            case 'm':
                mode = M_COUNT;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-w] [-c] [-m]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    open_file(argv[optind], mode);
    return 0;
}