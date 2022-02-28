//
//  string.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/28
//

#include "defs.h"

//参考Xcode中的函数参数, 以"__"开头的删除"__"

int memcmp(const void * s1, const void * s2, size_t n) {
    const unsigned char *v1, *v2;
    
    v1 = s1;
    v2 = s2;
    while (n-- > 0) {
        if (*v1 != *v2) {
            return *v1 - *v2;
        }
        v1++;
        v2++;
    }
    
    return 0;
}

void * memmove(void * dst, const void * src, size_t len) {
    const char *s;
    char *d;
    
    s = src;
    d = dst;
    if (s < d && s + len > d) {
        //说明目标d在s段的中间, 直接用s++和d++的方式会使部分空间被覆盖
        //所以要从结尾反着来
        s += len;
        d += len;
        while (len-- > 0) {
            *--d = *--s;
        }
    } else {
        while (len-- > 0) {
            *d++ = *s++;
        }
    }
    
    return dst;
}

// memcpy exists to placate GCC.  Use memmove.
void* memcpy(void *dst, const void *src, size_t n) {
  return memmove(dst, src, n);
}


void * memset(void * b, int c, size_t len) {
    //注意, 尽管c的类型是int, 本质上还是一个char
    char *d;
    
    d = b;
    while (len-- > 0) {
        *d++ = c;
    }
    
    return b;
}

char * safestrcpy(char * dst, const char * src, int n) {
    //参数参考strncpy()的参数名, 但是n可以小于0, 所以n是int类型
    char *os;

    os = dst;
    if (n <= 0) {
        return os;
    }
    while (n-- > 0 && (*dst++ = *src++) != 0);
    while (n-- > 0) {
        *dst++ = 0;
    }
    
    return os;
}

int strlen(const char * s) {
    int n;
    for (n = 0; s[n]; n++);
    return n;
}

int strncmp(const char * s1, const char * s2, size_t n) {
    while (n > 0 && *s1 && *s1 == *s2) {
        n--;
        s1++;
        s2++;
    }
    if (n == 0) {
        return 0;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

char * strncpy(char * dst, const char * src, size_t n) {
    char *os;

    os = dst;
    while (n-- > 0 && (*dst++ = *src++) != 0);
    while (n-- > 0) {
        *dst++ = 0;
    }
    
    return os;
}
