#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


//TODO: Realizar de forma recursiva para todos los archivos de un directorio

struct Extends {
    char *extension;
    char *commentSing;
};

char* getComment(char *ptr, size_t size, size_t index, char ch, FILE *file) {
    while((ch = fgetc(file)) != '\n') {
        size++;
        ptr = realloc(ptr, size);
        ptr[index] = ch;
        index++;
    }
    return &ptr[0];
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

    char *commentSing;
    int sizeSing;

    for(int i=0; i < lenOptions; i++) {
        if(!strcmp(options[i].extension, extensionFile)) {
            commentSing = options[i].commentSing;
            sizeSing = strlen(commentSing);
        }
    }
    

    FILE *f = fopen(argv[1], "r");

    if(f == NULL) {
        fprintf(stderr, "ERROR: %s\n", strerror(errno));
        exit(1);
    }

    char ch;
    char *sign, *subStr;
    char *comment = malloc(sizeof(char));
    char *todo = malloc(sizeof(char)); //Substring "TODO"
    char sub[5];
    size_t sizeComment, index, indexSub;
    do {
        memset(comment, 0, strlen(comment)); //Remove all the content of the string
        sizeComment = 1, index = 0;
        ch = fgetc(f);
        if(ch == *commentSing && sizeSing > 1) {
            strcat(sign, &ch);

            ch = fgetc(f);
            strcat(sign, &ch);

            if(!strcmp(sign, commentSing)) {
                comment = getComment(comment, sizeComment, index, ch, f);
                subStr = strchr(comment, ':');
                //printf("81: %s", subStr); 

                if(subStr != NULL) {
                    indexSub = (size_t)(subStr - comment); //find the index of the character
                    
                    memset(todo, 0, strlen(todo));
                    todo = realloc(todo, (indexSub+1));
                    strncpy(todo, &comment[0], indexSub); //Get the substring

                    strncpy(sub, &todo[0], 5);
                    if(!strcmp(sub, "TODO")) {
                        printf("%s\n", comment);
                    }
                }

            }
        } else if(ch == *commentSing) {
            comment = getComment(comment, sizeComment, index, ch, f);

            //TODO: make this whole block of code a function (101-114)
            subStr = strchr(comment, ':');
            //printf("81: %s", subStr); 

            if(subStr != NULL) {
                indexSub = (size_t)(subStr - comment); //find the index of the character
                
                memset(todo, 0, strlen(todo));
                todo = realloc(todo, (indexSub+1));
                strncpy(todo, &comment[0], indexSub); //Get the substring

                strncpy(sub, &todo[0], 5);
                if(!strcmp(sub, "TODO")) {
                    printf("%s\n", comment);
                }
            }
        }

    } while(ch != EOF);

    fclose(f);
    
    return 0;
}