/*
 * Author: Sam Brendel 10/28/2018
 * TCSS422 Professor Lloyd
 * Assignment 1: MASH Shell
 */

#ifndef TCSS422_MASH_H
#define TCSS422_MASH_H

#endif //TCSS422_MASH_H

#include <sys/types.h>
#include <stdio.h>

#define MAX_STRING_SIZE 255
#define MAX_DISPLAY_LINE_SIZE 80
char *mash1, *mash2, *mash3, *fileName, *buffer, *outputFile1, *outputFile2, *outputFile3;
char *tokenMash1[MAX_STRING_SIZE];
char *tokenMash2[MAX_STRING_SIZE];
char *tokenMash3[MAX_STRING_SIZE];
int forkFailures[3] = {0, 0, 0};
int returnValues[3] = {0, 0, 0};
long long startPid1, stopPid1, startPid2, stopPid2, startPid3, stopPid3, largestTime;
pid_t pid1, pid2, pid3;

void initialize();
void startMashShell();
void cleanup();
char* readFileContents(char *theFileName);
int getFileSize(FILE *file);
void displayLine(char* tokenString, long long number);
void displaySeparator();
char* createTokens(char *mashCommand, char *array[]);
void displayResults(char* tokenString, int number, long long time);
char* verifyStringSize(char* tempString, char* originalString);
void kickOffMash1();
void kickOffMash2();
void kickOffMash3();
void startFork(char* str1, char* str2, char* str3);
void displayChildrenPids(pid_t pid1, pid_t pid2, pid_t pid3);
void displayTotalElapsedTime();
void lastStuff(char* s1, char* s2, char* s3, long long timeP1, long long timeP2, long long timeP3);
long long timeInMilliseconds();
long long getLargest(long long num1, long long num2, long long num3);
void displayTotalElapsedTime(long long first, long long second);
void onReturn(pid_t pid);
void setFileName(char* fileName, int number);

