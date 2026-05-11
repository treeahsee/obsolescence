#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <wctype.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

enum {LINE_COUNT, WORD_COUNT, CHAR_COUNT, M_COUNT, DEFAULT} mode;

void print_func(char *file_name, int count_mode, int word_count, int line_count, size_t m_count,size_t char_count) {
    switch (count_mode) {
        case LINE_COUNT:
            printf("%d %s\n", line_count, file_name);
            break;
        case WORD_COUNT:
            printf("%d %s\n", word_count, file_name);
            break;
        case CHAR_COUNT:
            printf("%zu %s\n", char_count, file_name);
            break;
        case M_COUNT:
            printf("%zu %s\n", m_count, file_name);
            break;
        case DEFAULT:
            printf("%d %d %zu %s\n", line_count, word_count, char_count,  file_name);
            break;
    }
}

int word_cnt(char *line) {
    int cnt = 0;
    char *token = strtok(line, " \t\n\r");
    while(token != NULL) {
        cnt++;
        // printf("%s\n", token);
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

int counter(FILE *fp, char *file_name, int count_mode) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_count = 0, word_count = 0;
    size_t char_count = 0, m_count = 0;

    
    while ((read = getline(&line, &len, fp)) != -1) {
        switch(count_mode) {
            case LINE_COUNT:
                line_count++;
                break;
            case  WORD_COUNT:
                word_count += word_cnt(line);
                break;
            case CHAR_COUNT:
                char_count += read;
                break;
            case M_COUNT:
                m_count += m_cnt(line);
                break;
            case DEFAULT:
                line_count++;
                word_count += word_cnt(line); 
                char_count += read; 
                break;
            default:
                printf("hi\n");
                break;
        }
    }
    fclose(fp);

    if (line)
        free(line);
    
    print_func(file_name, count_mode, word_count, line_count, m_count, char_count);
    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    mode = DEFAULT;
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
                setlocale(LC_ALL, "");
                break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-w] [-c] [-m]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    FILE *fp = argv[optind] ? fopen(argv[optind], "r") : stdin;
    char *file_name = argv[optind] ? argv[optind] : "";   
    counter(fp, file_name, mode);
    return 0;
}