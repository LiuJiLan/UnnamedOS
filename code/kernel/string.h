//
//  string.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/25.
//

#ifndef string_h
#define string_h

#include "types.h"

int memcmp(const void *, const void *, size_t);
void* memmove(void *, const void *, size_t);
void* memset(void *, int, size_t);
char* safestrcpy(char*, const char*, int);
int strlen(const char *);
int strncmp(const char *, const char *, size_t);
char* strncpy(char *, const char *, size_t);


#endif /* string_h */
