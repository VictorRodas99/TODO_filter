#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


//TODO: Realizar de forma recursiva para todos los archivos de un directorio

typedef struct Extend {
    char *extension;
    char *commentSign;
} Extend;

typedef struct fComments {
    char *comment;
    size_t lvlPriority;
} fComments;

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

fComments verifyComment(char *comment) {
    char *subStr = strchr(comment, ':');
    char *todo = malloc(sizeof(char)); //Substring "TODO"
    char *totalComment = malloc(sizeof(char));
    char sub[5];
    fComments data;

    memset(todo, 0, strlen(todo));
    memset(totalComment, 0, strlen(todo));

    if(subStr != NULL) {
        size_t indexSub = (size_t)(subStr - comment); //find the index of the character
        size_t lenOfComment = strlen(comment) - (indexSub+2);

        todo = realloc(todo, indexSub);
        strncpy(todo, &comment[0], indexSub); //Get the substring

        totalComment = realloc(totalComment, lenOfComment); //Get the whole comment without "TODO" and ": "
        strncpy(totalComment, &comment[indexSub+2], lenOfComment);

        strncpy(sub, &todo[0], 4);
        if(!strcmp(sub, "TODO")) {
            size_t priority = strlen(todo) - strlen(sub);

            strcpy(data.comment, totalComment);
            data.lvlPriority = priority;

        } else { //if exists ':' into the comment but is not a 'TODO:'
            data.comment = "";
            data.lvlPriority = 100; 
        }

    } else {
        data.comment = "";
        data.lvlPriority = 100;
    }

    free(todo);
    free(totalComment);
    todo, totalComment = NULL;

    return data;
    
}

void getTotalData(fComments aux, char **totalComments, size_t *priority, int *countData, size_t *indexData) { 

    if(aux.lvlPriority < 100)  {
        *countData = *countData + 1;

        totalComments = realloc(totalComments, sizeof(char*) * (*countData));

        if(totalComments == NULL) {
            printf("No allocated\n");
            exit(1);
        }

        totalComments[*indexData] = malloc(sizeof(char) * strlen(aux.comment));
        priority = realloc(priority, sizeof(size_t) * (*countData));

        strcpy(totalComments[*indexData], aux.comment);
        priority[*indexData] = aux.lvlPriority; //doesn't work

        *indexData = *indexData + 1;
    }

}

int main(int argc, char *argv[]) {

    if(argc < 2) {
        printf("ERROR: No file name were given\n");
        exit(1);
    }

    Extend options[4] = {
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
    

    FILE *f = fopen(argv[1], "r");

    if(f == NULL) {
        fprintf(stderr, "ERROR: %s\n", strerror(errno));
        exit(1);
    }

    if(!control) {
        printf("ERROR: files with %s extension are not implemented yet\n", extensionFile);
        exit(1);
    }

    char ch;
    char *auxCh = malloc(sizeof(char));
    char *sign = malloc(sizeof(char));
    char *comment = malloc(sizeof(char));
    size_t sizeComment, index, indexData = 0;
    int countData = 0;

    char **totalComments = calloc(sizeof(char*), 1);
    size_t *priority = calloc(sizeof(size_t), 1);
    fComments aux;
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
                aux = verifyComment(comment);

                getTotalData(aux, totalComments, priority, &countData, &indexData);
                memset(&aux, 0, sizeof(aux)); //Reset the auxiliar structure

            }
        } else if(ch == *commentSign) {
            comment = getComment(comment, sizeComment, index, ch, f);
            aux = verifyComment(comment);

            getTotalData(aux, totalComments, priority, &countData, &indexData);
            memset(&aux, 0, sizeof(aux));
        }

    } while(ch != EOF);

    //Show all the comments with their level of priority
    for(size_t x=0; x < indexData; x++) {
        printf("%s (lvl: %ld)\n", totalComments[x], priority[x]);
    }
    
    int y = 0;
    while(y < indexData) {
        free(totalComments[y]);
        totalComments[y] = NULL;
        y++;
    }

    free(totalComments);
    free(priority);
    free(comment);
    free(auxCh);
    free(sign);
    auxCh, sign, comment, totalComments, priority = NULL;

    fclose(f);
    
    return 0;
}