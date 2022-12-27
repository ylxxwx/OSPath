#ifndef STRINGS_H
#define STRINGS_H

void reverse(char s[]);
int strlen(char s[]);
int backspace(char s[]);
void append(char s[], char n);
int strcmp(char s1[], char s2[]);
char * kstrcpy(char *dst, const char *src);
char *strtok(char *s1, const char *s2);
int split(char dst[][80], char* str, char *spl);

void memset(char* buf, char val, int size);
#endif
