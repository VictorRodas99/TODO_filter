#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


//TODO: Realizar de forma recursiva para todos los archivos de un directorio

struct Extends {
    char *extension;
    char *commentSign;
};

char* getComment(char *ptr, size_t size, size_t index, char ch, FILE *file) {
    while((ch = fgetc(file)) != '\n') {
        if(ch == EOF) { break; }

        size++;
        ptr = realloc(ptr, size);
        ptr[index] = ch;
        index++;
    }
    return &ptr[0];
}

int verifyComment(char *comment) {
    char *subStr = strchr(comment, ':');
    char *todo = malloc(sizeof(char)); //Substring "TODO"
    int status = 0;
    char sub[5];

    memset(todo, 0, strlen(todo));

    if(subStr != NULL) {
        size_t indexSub = (size_t)(subStr - comment); //find the index of the character
    
        todo = realloc(todo, (indexSub+1));
        strncpy(todo, &comment[0], indexSub); //Get the substring

        strncpy(sub, &todo[0], 5);
        if(!strcmp(sub, "TODO")) {
            status = 1; 
        }
    }

    free(todo);
    todo = NULL;

    return status;
    
}

int main(int argc, char *argv[]) {

    if(argc < 2) {
        printf("ERROR: No file name were given\n");
        exit(1);
    }

    struct Extends options[4] = {
        {".py", "#"}, {".c", "//"},
        {".js", "//"}, {".cpp", "//"}
    };

    size_t lenOptions = sizeof(options) / sizeof(options[0]);
    char *extensionFile = strrchr(argv[1], '.');

    if(extensionFile == NULL) {
        printf("ERROR: Invalid file name\n");
        exit(1);
    }

    char *commentSign;
    int sizeSign;
    int control = 0;

    for(int i=0; i < lenOptions; i++) {
        if(!strcmp(options[i].extension, extensionFile)) {
            commentSign = options[i].commentSign;
            sizeSign = strlen(commentSign);
            control = 1;
        }
    }

    if(!control) {
        printf("ERROR: files with %s extension are not implemented yet\n", extensionFile);
        exit(1);
    }
    

    FILE *f = fopen(argv[1], "r");

    if(f == NULL) {
        fprintf(stderr, "ERROR: %s\n", strerror(errno));
        exit(1);
    }

    char ch;
    char *auxCh = malloc(sizeof(char));
    char *sign = malloc(sizeof(char));
    char *comment = malloc(sizeof(char));
    int status;
    size_t sizeComment, index;
    do {
        memset(auxCh, 0, strlen(auxCh));
        memset(sign, 0, strlen(sign));
        memset(comment, 0, strlen(comment)); //Remove all the content of the string

        sizeComment = 1, index = 0;
        ch = fgetc(f);
        if(ch == *commentSign && sizeSign > 1) {
            auxCh[0] = ch; //We pass the char into an auxiliar bc we can't concat char(int) with a literal str
            strcat(sign, auxCh);

            ch = fgetc(f);
            auxCh[0] = ch;
            sign = realloc(sign, 2);
            strcat(sign, auxCh);

            if(!strcmp(sign, commentSign)) {
                comment = getComment(comment, sizeComment, index, ch, f);
                status = verifyComment(comment);

                if(status) {
                    printf("%s\n", comment);
                }

            }
        } else if(ch == *commentSign) {
            comment = getComment(comment, sizeComment, index, ch, f);
            status = verifyComment(comment);

            if(status) {
                printf("%s\n", comment);
            }
        }

    } while(ch != EOF);


    free(comment);
    free(auxCh);
    free(sign);
    auxCh, sign, comment = NULL;

    fclose(f);
    
    return 0;
}