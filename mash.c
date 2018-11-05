/*
 * Author: Sam Brendel 10/28/2018
 * TCSS422 Professor Lloyd
 * Assignment 1: MASH Shell
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include "mash.h"

int main(int argc, char **argv) {
    initialize();
    startMashShell();
    char *str1 = createTokens(mash1, tokenMash1);
    char *str2 = createTokens(mash2, tokenMash2);
    char *str3 = createTokens(mash3, tokenMash3);
    startFork(str1, str2, str3);
    return 0;
}

void initialize() {
    mash1 = (char *) malloc(sizeof(char) * MAX_STRING_SIZE);
    mash2 = (char *) malloc(sizeof(char) * MAX_STRING_SIZE);
    mash3 = (char *) malloc(sizeof(char) * MAX_STRING_SIZE);
    fileName = malloc(sizeof(char) * MAX_STRING_SIZE);
}

void startMashShell() {
    //char *fileName;
    char *temp1 = malloc(sizeof(char) * MAX_STRING_SIZE);
    char *temp2 = malloc(sizeof(char) * MAX_STRING_SIZE);
    char *temp3 = malloc(sizeof(char) * MAX_STRING_SIZE);

    printf("mash-1>");
    //gets(temp1);
    fgets(temp1, sizeof(temp1), stdin);
    temp1[strcspn(temp1, "\n")] = 0;
    mash1 = verifyStringSize(temp1, mash1); // re-size if longer than MAX_STRING_SIZE.
    temp1 = NULL; // do not free it, because that memory is needed elsewhere.  Just set the pointer to null.

    printf("mash-2>");
    //gets(temp2);
    fgets(temp2, sizeof(temp2), stdin);
    temp2[strcspn(temp2, "\n")] = 0;
    mash2 = verifyStringSize(temp2, mash2);
    temp2 = NULL;

    printf("mash-3>");
    //gets(temp3);
    fgets(temp3, sizeof(temp3), stdin);
    temp3[strcspn(temp3, "\n")] = 0;
    mash3 = verifyStringSize(temp3, mash3);
    temp3 = NULL;
    printf("file>");
    //gets(fileName);
    fgets(fileName, sizeof(fileName), stdin);
    fileName[strcspn(fileName, "\n")] = 0;

}

void startFork(char* str1, char* str2, char* str3) {

    startPid1 = timeInMilliseconds();
    pid1 = fork();
    if (pid1 < 0) {
        forkFailures[0] = pid1;
        printf("pid1 fork failed\n");
        exit(1);
    } else if (pid1 == 0) {         // CHILD1
        //printf("I'm the 1st child (pid:%d)\n", (int)getpid());
        kickOffMash1();

    } else if (pid1 > 0) {          // PARENT OF CHILD1
        startPid2 = timeInMilliseconds();
        pid2 = fork();
        if (pid2 < 0) {
            forkFailures[1] = pid2;
            printf("pid2 fork failed\n");
            exit(1);
        } else if (pid2 == 0) {     // CHILD2
            //printf("I'm the 2nd child (pid:%d)\n", (int)getpid());
            kickOffMash2();

        } else if (pid2 > 0) {      // PARENT OF CHILD2
            startPid3 = timeInMilliseconds();
            pid3 = fork();
            if (pid3 < 0) {
                forkFailures[2] = pid3;
                printf("pid3 fork failed\n");
                exit(1);
            } else if (pid3 == 0) { // CHILD3
                //printf("I'm the 3rd child (pid:%d)\n", (int)getpid());
                kickOffMash3();
            } else if (pid3 > 0){   // PARENT OF CHILD3
                //printf("hello, I am the parent of the child and am exiting now...\n"); // TESTING
                onReturn(wait(NULL));
                onReturn(wait(NULL));
                onReturn(wait(NULL));
                largestTime = getLargest(stopPid1, stopPid2, stopPid3);
                lastStuff(str1, str2, str3, (stopPid1-startPid1), (stopPid2-startPid2), (stopPid3-startPid3));
                displaySeparator();
                displayChildrenPids(pid1, pid2, pid3);
                displayTotalElapsedTime(startPid1, largestTime);
                //printf("startPid1=%lld   stoppid1=%lld\n", startPid1, stoppid1);
                //printf("startPid1=%lld   stoppid2=%lld\n", startPid1, stoppid2);
                //printf("startpid3=%lld   stoppid3=%lld\n", startpid3, stoppid3);
            }
        }
    }

}

void kickOffMash1() {
    int new_file_handler = 0;
    close(STDOUT_FILENO);
    new_file_handler = open("mash1.output", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    int statusCode = execvp(tokenMash1[0], tokenMash1);
    fprintf(stdout, "CMD1:[SHELL 1] STATUS CODE=%d\n", statusCode);
}

void kickOffMash2() {
    int new_file_handler = 0;
    close(STDOUT_FILENO);
    new_file_handler = open("mash2.output", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    int statusCode = execvp(mash2, tokenMash2);
    fprintf(stdout, "CMD2:[SHELL 2] STATUS CODE=%d\n", statusCode);
}

void kickOffMash3() {
    int new_file_handler = 0;
    close(STDOUT_FILENO);
    new_file_handler = open("mash3.output", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    int statusCode = execvp(mash3, tokenMash3);
    fprintf(stdout, "CMD3:[SHELL 3] STATUS CODE=%d\n", statusCode);
}

void onReturn(pid_t pid) {
    if (pid == pid1) {
        stopPid1 = timeInMilliseconds();
        printf("First process finished...\n");
    } else if (pid == pid2) {
        printf("Second process finished...\n");
        stopPid2 = timeInMilliseconds();
    } else if (pid == pid3) {
        printf("Third process finished...\n");
        stopPid3 = timeInMilliseconds();
    } else {
        //printf("Error running process on some pid.  Returned %d\n", pid);
    }
}

char* readFileContents(char *theFileName) {
    FILE *fp = fopen(theFileName, "r");
    if (fp != NULL) {
        /*while(!feof(fp)) {
            fgets(string, MAX_STRING_SIZE, fp);
            strcat(fileContents, string);
        }*/
        int fileSize = getFileSize(fp);
        char* buf = (char *) calloc(fileSize, sizeof(char));
        char string[MAX_STRING_SIZE] = "";
        fread(buf, sizeof(char), fileSize, fp);
        fclose(fp);
        return buf;
    } else {
        printf("\n ERROR: file does not exist.\n");
    }
    return NULL;
}

int getFileSize(FILE *file) {
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return size;
}

char* verifyStringSize(char* tempString, char* originalString) {
    int multiplier = 1;
    long length = strlen(tempString);
    char *newString;
    for (int i=0; length > (multiplier * MAX_STRING_SIZE); i++) {
        multiplier++;
    }
    if (multiplier > 1) {
        newString = malloc(sizeof(char) * multiplier * MAX_STRING_SIZE);
        strcpy(newString, tempString);
        free(originalString);
        //originalString = newString;
    } else {
        strcpy(originalString, tempString);
        newString = originalString;
    }
    return newString;
}

char* createTokens(char *mashCommand, char *array[]) {
    // Consider unlimited quantity of parameters for the command.
    /*char **newArray; //2 dimensional array pointer.
    int multiplier = 1;
    int length = strlen(mashCommand);
    if (length > MAX_STRING_SIZE) {
        for (int i = multiplier; length > (MAX_STRING_SIZE * multiplier); multiplier++) {
            //
        }
        free(array);
        array = malloc(multiplier * MAX_STRING_SIZE * sizeof(char));
    }*/ // TODO This block does not accomplish what I need it to because mashCommand is string is limited in length upon the user entering input.

    // Splitting up the tokens and populating the array.
    array[0] = strtok(mashCommand, " ");
    char* token = strtok(NULL, " "); // advance to the next token. NO NO NO!!! KEEP THE COMMAND IN THE ARRAY!
    int i=1;
    for (; token != NULL; i++) {
        array[i] = token;
        token = strtok(NULL, " "); // advance to the next token.
    }
    array[i] = NULL; // The array must be null terminating in order for
    // the execvp command to use it.

    mashCommand = array[0]; // replace the mash# with only the first token.

    char *tok = (char *) malloc(i * sizeof(char) * MAX_STRING_SIZE);
    tok[0] = '\0';
    int j=0;
    for (; j<i; ++j) {
        strcat(tok, " ");
        strcat(tok, array[j]);
    }
    array[j] = fileName;
    //array = newArray;
    return tok;
}

void displayLine(char *tokenString, long long number) {
    char intToString[1] = "\0";
    sprintf(intToString, "%lld", number);
    char commandPrefix[] = "-----CMD \0";
    strcat(commandPrefix, intToString);
    strcat(commandPrefix, ":\0");
    long sizeCommandPrefix = strlen(commandPrefix);
    long sizeTokenString = strlen(tokenString);
    long quantity = MAX_DISPLAY_LINE_SIZE - sizeCommandPrefix - sizeTokenString;

    printf("%s%s", commandPrefix, tokenString);
    if (quantity > 0) {
        for (int j = 0; j < quantity; j++) {
            printf("-");
        }
    }
    printf("\n");
}

void displaySeparator() {
    for (int i = 0; i < MAX_DISPLAY_LINE_SIZE; i++) {
        printf("-");
    }
    printf("\n");
}

void displayResults(char *tokenString, int number, long long time) {
    displayLine(tokenString, number);
    if (forkFailures[number - 1] < 0) {
        printf("CMD%d:[SHELL %d] STATUS CODE=%d\n", number, number, returnValues[number - 1]);
    } else {
        // Only if the command successfully ran, display the output.
        char fileName[20] = "mash\0";
        char intToString[2] = " \0";
        sprintf(intToString, "%d", number);
        strcat(fileName, intToString);
        strcat(fileName, ".output\0");
        char* output = readFileContents(fileName);
        strcat(output, "\0"); // The string must be null terminating for printf().
        printf("%s", output);
        setFileName(fileName, number);
        printf("Result took:%lldms\n", time);
    }
}

void setFileName(char* fileName, int number) {
    switch(number) {
        case 1:
            outputFile1 = malloc(sizeof(char) * strlen(fileName)+1);
            strcpy(outputFile1, fileName);
            break;
        case 2:
            outputFile2 = malloc(sizeof(char) * strlen(fileName)+1);
            strcpy(outputFile2, fileName);
            break;
        case 3:
            outputFile3 = malloc(sizeof(char) * strlen(fileName)+1);
            strcpy(outputFile3, fileName);
            break;
    }
}

long long getLargest(long long num1, long long num2, long long num3) {
    long long result = num1;
    if (result < num2) {
        result = num2;
    }
    if (result < num3) {
        result = num3;
    }
    return result;
}

void lastStuff(char* s1, char* s2, char* s3, long long timepid1, long long timepid2, long long timepid3) {
    displayResults(s1, 1, timepid1);
    displayResults(s2, 2, timepid2);
    displayResults(s3, 3, timepid3);
    cleanup();
    free(s1);
    free(s2);
    free(s3);
}

void displayChildrenPids(pid_t pid1, pid_t pid2, pid_t pid3) {
    printf("Children process IDs: %d %d %d.\n", pid1, pid2, pid3);
}

void displayTotalElapsedTime(long long first, long long second) {
    printf("Total elapsed time:%lldms\n", (second-first));
}

long long timeInMilliseconds() {
    struct timeval time;
    gettimeofday(&time,NULL);
    return (((long long)time.tv_sec)*1000)+(time.tv_usec/1000);
}

void cleanup() {
    remove(outputFile1);
    remove(outputFile2);
    remove(outputFile3);
    free(outputFile1);
    free(outputFile2);
    free(outputFile3);
    free(mash1);
    free(mash2);
    free(mash3);
    free(buffer);
}

