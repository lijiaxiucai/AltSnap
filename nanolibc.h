#ifndef NANOLIBC_H
#define NANOLIBC_H

/* return +/-1 if x is +/- and 0 if x == 0 */
static inline int sign(int x)
{
    return (x > 0) - (x < 0);
}

// branchless min/max functions for int32
static inline int Imin(int a, int b)
{ 
    return b + ((a-b) & (a-b)>>31);
}
static inline int Imax(int a, int b)
{ 
    return a - ((a-b) & (a-b)>>31);
}

void *memsetL(void *dst, int s, size_t count) 
{
    register char * a = dst;
    count++;
    while (--count) 
        *a++ = s;
    return dst;
}
/* #define memset memsetL */ 


static inline wchar_t *wcschrL(wchar_t *__restrict__ str, const wchar_t c)
{
    while(*str != c) {
        if(!*str) return NULL;
        str++;
    }
    return str;
}
#define wcschr wcschrL

static inline const char *strchrL(const char *__restrict__ str, const char c)
{
    while(*str != c) {
        if(!*str) return NULL;
        str++;
    }
    return str;
}
#define strchr strchrL

static int wtoiL(const wchar_t *s)
{
    long int v=0;
    int sign=1;
    while (*s == ' ') s++; /*  ||  (unsigned int)(*s - 9) < 5u */
    
    switch (*s) {
    case '-': sign=-1; /* fall through */
    case '+': ++s;
    }
    while ((unsigned)(*s - '0') < 10u) {
        v = v * 10 + (*s - '0');
        ++s;
    }
    return sign*v;
}
#define _wtoi wtoiL

static inline void reverse(wchar_t *str, int length)
{
    int start = 0;
    int end = length -1;
    while (start < end) {
        wchar_t tmp;
        tmp = str[start];
        str[start] = str[end];
        str[end] = tmp;
        start++;
        end--;
    }
}
static wchar_t *itowL(int num, wchar_t *str, int base)
{
    int i = 0;
    int isNegative = 0;

    /* Handle 0 explicitely */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    /* In standard itoa(), negative numbers are handled only with
     * base 10. Otherwise numbers are considered unsigned. */
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }

    /* Process individual digits */
    while (num != 0) {
        int rem = num % base;
        /* str[i++] = (rem > 9)? (rem-10) + 'A' : rem + '0'; */
        str[i++] = rem + '0' + (rem > 9) * ('A' - '0' - 10); /* branchless version */

        num = num/base;
    }

    /* If number is negative, append '-' */
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; /* Append string terminator */

    /* Reverse the string */
    reverse(str, i);

    return str;
}
#define _itow itowL

static inline size_t wcslenL(wchar_t *__restrict__ str)
{
    wchar_t *ptr;
    for (ptr=str; *ptr != '\0'; ptr++);
    return ptr-str;
}
#define wcslen wcslenL

static inline int wcscmpL(const wchar_t *__restrict__ a, const wchar_t *__restrict__ b)
{
    while(*a && *a == *b) { a++; b++; }
    return *a - *b;
}
#define wcscmp wcscmpL

static inline wchar_t *wcscpyL(wchar_t *__restrict__ dest, const wchar_t *__restrict__ in)
{
    wchar_t *ret = dest;
    while ((*dest++ = *in++));
    return ret;
}
#define wcscpy wcscpyL

static wchar_t *wcsncpyL(wchar_t *__restrict__ dest, const wchar_t *__restrict__ src, size_t n)
{
    wchar_t *orig = dest;
    for (; dest < orig+n && (*dest=*src); ++src, ++dest) ;
    for (; dest < orig+n; ++dest) *dest=0;
    return orig;
}
#define wcsncpy wcsncpyL

static inline char *strcpyL(char *__restrict__ dest, const char *__restrict__ in)
{
    char *ret = dest;
    while ((*dest++ = *in++));
    return ret;
}
#define strcpy strcpyL

static int wcsicmpL(const wchar_t* s1, const wchar_t* s2)
{
    unsigned x1, x2;

    while (1) {
        x2 = *s2 - 'A';
        x2 |= (x2 < 26u) << 5; /* Add 32 if UPPERCASE. */

        x1 = *s1 - 'A';
        x1 |= (x1 < 26u) << 5;

        s1++; s2++;
        if (x2 != x1)
            break;
        if (x1 == (unsigned)-'A')
            break;
    }
    return x1 - x2;
}
#define wcsicmp wcsicmpL

wchar_t* wcscat(wchar_t *__restrict__ dest, const wchar_t *__restrict__ src)
{
    wchar_t *orig=dest;
    for (; *dest; ++dest) ;	/* go to end of dest */
    for (; (*dest=*src); ++src,++dest) ;	/* then append from src */
    return orig;
}

static inline int strcmpL(const char *X, const char *Y)
{
    while (*X && *X == *Y) {
        X++;
        Y++;
    }
    return *(const unsigned char*)X - *(const unsigned char*)Y;
}
#define strcmp strcmpL

static const wchar_t *wcsstrL(const wchar_t *haystack, const wchar_t *needle)
{
    size_t i,j;
    for (i=0; haystack[i]; ++i) {
        for (j=0; haystack[i+j]==needle[j] && needle[j]; ++j) ;
        if (!needle[j]) return &haystack[i];
    }
    return NULL;
}
#define wcsstr wcsstrL

static const char *strstrL(const char *haystack, const char *needle)
{
    size_t i,j;
    for (i=0; haystack[i]; ++i) {
        for (j=0; haystack[i+j]==needle[j] && needle[j]; ++j) ;
        if (!needle[j]) return &haystack[i];
    }
    return NULL;
}
#define strstr strstrL

static inline unsigned h2u(const wchar_t c)
{
    if      (c >= '0' && c <= '9') return c - '0';
    else if (c >= 'A' && c <= 'F') return c-'A'+10;
    else if (c >= 'a' && c <= 'f') return c-'a'+10;
    else return 0;
}
static unsigned whex2u(const wchar_t s[2])
{
    if(h2u(s[1])) return h2u(s[0]) << 4 | h2u(s[1]);
    else return h2u(s[0]);
}

static inline void *reallocL(void *mem, size_t sz)
{
    if(!mem) return HeapAlloc(GetProcessHeap(), 0, sz);
    return HeapReAlloc(GetProcessHeap(), 0, mem, sz);
}
#define realloc reallocL

static inline void *mallocL(size_t sz)
{
    return HeapAlloc(GetProcessHeap(), 0, sz);
}
#define malloc mallocL

static inline void *callocL(size_t sz, size_t mult)
{
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sz*mult);
}
#define calloc callocL

static inline BOOL freeL(void *mem)
{
    return HeapFree(GetProcessHeap(), 0, mem);
}
#define free freeL

#endif