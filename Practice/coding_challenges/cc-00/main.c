#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <wctype.h>

enum {LINE_COUNT, WORD_COUNT, CHAR_COUNT} mode = LINE_COUNT;


int open_file(char *file_name, int count_mode) {
    FILE *fp;
    int c;
    int line_count = 0;
    int word_count = 0;
    int char_count = 0;

    fp = fopen(file_name, "r");
    
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n')
            line_count++;
        if (iswspace(c)) // wrong im likely counting consecutive white space
            word_count++;
        char_count++;
    }

    printf("%d\t%d\t%d\n", line_count, word_count, char_count);
    
    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "lwc")) != -1) {
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
            default:
                fprintf(stderr, "Usage: %s [-l] [-w] [-c]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    open_file(argv[optind], mode);
    return 0;
}