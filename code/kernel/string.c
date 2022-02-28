//
//  string.c
//  xv6-qemu
//
//  Created by KeZiJie and LiXuHao on 2022/1/18
//

//  函数在defs中声明
//  注意在现代标准中有的使用的是size_t, 之后可以更完善

#define NULL 0x0
// 理论上是不能用NULL的, 之后再让组员把这里改精细一些吧

void *memset(void *dst, int c, unsigned long len)
{
    if (dst == NULL || len < 0)
    {
        return NULL;
    }
    char *pdst = (char *)dst;
    while (len-- > 0)
    {
        *pdst++ = c;
    }
    return dst;
}

int memcmp(const void *v1, const void *v2, unsigned long n)
{
    const unsigned char *su1, *su2;
    int res = 0;
    for (su1 = v1, su2 = v2; 0 < n; ++su1, ++su2, n--)
        if ((res = *su1 - *su2) != 0)
            break;
    return res;
}

void * memmove(void * dst, const void * src, unsigned long n)
{
    void * ret = dst;
    if (dst <= src || (char *)dst >= ((char *)src + n))
    {
        //若dst和src区域没有重叠，则从起始处开始逐一拷贝
        while (n--)
        {
            *(char *)dst = *(char *)src;
            dst = (char *)dst + 1;
            src = (char *)src + 1;
        }
    }
    else
    {
        // 若dst和src 区域交叉，则从尾部开始向起始位置拷贝，这样可以避免数据冲突
        dst = (char *)dst + n - 1;
        src = (char *)src + n - 1;
        while (n--)
        {
            *(char *)dst = *(char *)src;
            dst = (char *)dst - 1;
            src = (char *)src - 1;
        }
    }
    return (ret);
}

void* memcpy(void *dst, const void *src, unsigned long n)
{
    return memmove(dst, src, n);
}

int strncmp(const char *p, const char *q, unsigned long n)
{
    signed char res = 0;
    while (n--)
    {
    //  比较到结束符/0，时，已经做了res = *p - *q了，所以不等长度时，肯定返回不为0
        if ((res = *p - *q++) != 0 || !*p++)
            break;
    }
    return res;
}

char *strncpy(char *s, const char *t, unsigned long n)
{
    if(s == NULL || t == NULL || n < 0)
        return NULL;
    int i = 0;
    char *temp = s;
    for (i = 0; i < n; i++)
    {
        *temp++ = *t++;
    }
    return s;
}

char* safestrcpy(char* dst, const char* src, int n)
{
    //  未实现
    return 0;   //  单纯为了消除warning
}

int strlen(const char *s)
{
    int len=0;
    if(s == NULL)
        return 0;
    while(*s++!='\0')
    {
        len++;
    }
    return len;
}
