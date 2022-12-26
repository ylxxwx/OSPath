#include "string.h"
#include "../cpu/type.h"

#define NULL 0
/*void int_to_ascii(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

void hex_to_ascii(int n, char str[]) {
    append(str, '0');
    append(str, 'x');
    char zeros = 0;

    s32 tmp;
    int i;
    for (i = 28; i > 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) continue;
        zeros = 1;
        if (tmp > 0xA) append(str, tmp - 0xA + 'a');
        else append(str, tmp + '0');
    }

    tmp = n & 0xF;
    if (tmp >= 0xA) append(str, tmp - 0xA + 'a');
    else append(str, tmp + '0');
}
*/
/* K&R */
void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* K&R */
int strlen(char s[]) {
    int i = 0;
    while (s[i] != '\0') ++i;
    return i;
}

void append(char s[], char n) {
    int len = strlen(s);
    s[len] = n;
    s[len+1] = '\0';
}

int backspace(char s[]) {
    int len = strlen(s);
    if (len > 0) {
        s[len-1] = '\0';
        return 0;
    }
    return -1;
}

/* K&R 
 * Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2 */
int strcmp(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}

char* kstrcpy(char* destination, const char* source)
{
    // return if no memory is allocated to the destination
    if (destination == NULL) {
        return NULL;
    }
 
    // take a pointer pointing to the beginning of the destination string
    char *ptr = destination;
 
    // copy the C-string pointed by source into the array
    // pointed by destination
    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }
 
    // include the terminating null character
    *destination = '\0';
 
    // the destination is returned by standard `strcpy()`
    return ptr;
}

typedef u32 size_t;
#define NULL 0

size_t czf_strcspn(const char *s1, const char *s2) {
    const char *sc1, *sc2;
    for (sc1 = s1; *sc1 != '\0'; ++sc1) {
        for (sc2 = s2; *sc2 != '\0'; ++sc2) {
            if (*sc1 == *sc2)
                return (sc1 - s1);
        }
    }
    return (sc1 - s1); // terminating nulls match.
}

/* find index of first s1[i] that matches no s2[]. */
size_t czf_strspn(const char *s1, const char *s2) {
    const char *sc1, *sc2;
    for (sc1 = s1; *sc1 != '\0'; ++sc1) {
        for (sc2 = s2; ; ++sc2) {
            if (*sc2 == '\0') {
                return (sc1 - s1);
            } else if (*sc1 == *sc2) {
                break;
            }
        }
    }   
    return (sc1 - s1);
}

/* find next token in s1[] delimited by s2[]. */
char *strtok(char *s1, const char *s2) {
    char *sbegin, *send;
    static char *ssave = "";
    
    sbegin = s1 ? s1 : ssave;
    sbegin += czf_strspn(sbegin, s2);
    
    if (*sbegin == '\0') {
        ssave = "";
        return NULL;
    }
    
    send = sbegin + czf_strcspn(sbegin, s2);
    if (*send != '\0')
        *send++ = '\0';
        
    ssave = send;
    return (sbegin);
}
