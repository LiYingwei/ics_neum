/*
The MIT License (MIT)

Copyright (c) 2015 Zhang Boyang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

char input_buffer[] = 
"\x32\x0A\x34\x20\x31\x20\x31\x0A\x31\x0A"
"\x32\x0A\x33\x20\x32\x20\x33\x0A\x31\x20"
"\x31\x0A\x31\x20\x31\x0A"
;

char answer_buffer[] = 
"\x31\x35\x0A\x34\x34\x0A"
;

#include "trap.h"
#include <stdarg.h>
#include <string.h>

/* TEMPLATE CODE */

char output_buffer[sizeof(answer_buffer) + 0x100];


/* only usable when a >= 0 */
void LLdiv10(unsigned long long a, unsigned long long *q, int *r)
{
    union {
        long long value;
        unsigned short s[4];
    } LL;
    
    int i;
    unsigned int cur, sq, sr = 0;
    LL.value = a;

    for (i = 3; i >= 0; i--) {
        cur = sr * 0x10000 + LL.s[i];
        sq = cur / 10;
        sr = cur % 10;
        LL.s[i] = sq;
    }

    *q = LL.value;
    *r = sr;
}

/* useful functions */

int naive_isdigit(char c)
{
    return '0' <= c && c <= '9';
}

int naive_isspace(char c)
{
    switch (c) {
        case ' ': case '\f': case '\n': case '\r': case '\t': case '\v':
            return 1;
        default:
            return 0;
    }
}

/* write_TYPE(): print TYPE to output buffer
 * return outputed char cnt
 */

char *write_ptr = output_buffer;

void finish_output()
{
    *write_ptr = '\0';
}

int write_char(char c)
{
    *write_ptr++ = c;
    return 1;
}

int write_string(char *s)
{
    int cnt = 0;
    while (*s) cnt += write_char(*s++);
    return cnt;
}

int write_llint(long long lld)
{
    int buf[100];
    int p = 0, ret;
    unsigned long long llu;
    if (lld < 0) { write_char('-'); lld = -lld; }
    llu = lld;
    do {
        LLdiv10(llu, &llu, &    buf[p++]);
    } while (llu > 0);
    ret = p;
    while (p > 0) write_char('0' + buf[--p]);
    return ret;
}

int write_int(int d)
{
    char buf[100];
    int p = 0, ret;
    unsigned int u;
    if (d < 0) { write_char('-'); d = -d; }
    u = d;
    do {
        buf[p++] = u % 10;
        u /= 10;
    } while (u > 0);
    ret = p;
    while (p > 0) write_char('0' + buf[--p]);
    return ret;
}

int naive_printf(const char *fmt, ...)
{
    va_list ap;
    long long lld;
    int d;
    char c, *s;
    int cnt = 0;
    
    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt == '%') {
            switch (*++fmt) {
                case 's':
                    s = va_arg(ap, char *);
                    cnt += write_string(s);
                    break;
                case 'd':
                    d = va_arg(ap, int);
                    cnt += write_int(d);
                    break;
                case 'c':
                    c = va_arg(ap, int);
                    cnt += write_char(c);
                    break;
                case 'l':
                    if (*++fmt == 'l' && *++fmt == 'd') {
                        lld = va_arg(ap, long long);
                        cnt += write_llint(lld);
                    }
                    break;
                case '%':
                    cnt += write_char('%');
                    break;
            }
        } else {
            cnt += write_char(*fmt);
        }
        fmt++;
    }
    va_end(ap);
    return cnt;
}


/* read_TYPE(): read TYPE from input buffer
 * return read object count
 */

char *read_ptr = input_buffer;

int read_char(char *cp)
{
    if (*read_ptr) {
        *cp = *read_ptr++;
        return 1;
    } else {
        return 0;
    }
}

void unread_char(char c)
{
    *--read_ptr = c;
}

void read_space()
{
    char c;
    while (read_char(&c) && naive_isspace(c));
    unread_char(c);
}

int read_string(char *s)
{
    int flag = 0;
    char c;
    read_space();
    while (read_char(&c) && !naive_isspace(c)) {
        *s++ = c;
        flag = 1;
    }
    unread_char(c);
    if (flag) *s = '\0';
    return flag;
}

int read_llint(long long *lldp)
{
    int flag = 0;
    long long lld = 0;
    int f = 0;
    char c;
    read_space();
    read_char(&c);
    if (c == '-') f = 1; else unread_char(c);
    while (read_char(&c) && naive_isdigit(c)) {
        lld = lld * 10 + (c - '0');
        flag = 1;
    }
    unread_char(c);
    if (flag) *lldp = f ? -lld : lld;
    if (!flag && f) unread_char('-');
    return flag;
}

int read_int(int *dp)
{
    int flag = 0;
    int d = 0;
    int f = 0;
    char c;
    read_space();
    read_char(&c);
    if (c == '-') f = 1; else unread_char(c);
    while (read_char(&c) && naive_isdigit(c)) {
        d = d * 10 + (c - '0');
        flag = 1;
    }
    unread_char(c);
    if (flag) *dp = f ? -d : d;
    if (!flag && f) unread_char('-');
    return flag;
}

int naive_scanf(const char *fmt, ...)
{
    va_list ap;
    long long *lldp;
    int *dp;
    char c, *cp, *s;
    int cnt = 0;
    int flag = 0;
    
    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt == '%') {
            switch (*++fmt) {
                case 's':
                    s = va_arg(ap, char *);
                    cnt += flag = read_string(s);
                    break;
                case 'd':
                    dp = va_arg(ap, int *);
                    cnt += flag = read_int(dp);
                    break;
                case 'c':
                    cp = va_arg(ap, char *);
                    cnt += flag = read_char(cp);
                    break;
                case 'l':
                    if (*++fmt == 'l' && *++fmt == 'd') {
                        lldp = va_arg(ap, long long *);
                        cnt += flag = read_llint(lldp);
                    }
                    break;
                case '%':
                    read_char(&c);
                    flag = (c == '%');
                    break;
            }
        } else {
            read_char(&c);
            flag = (c == *fmt);
        }
        if (!flag) goto done;
        fmt++;
    }
done:
    va_end(ap);
    return cnt;
}



int naive_memcmp(void *p1, void *p2, int n)
{
    unsigned char *s1 = p1, *s2 = p2;
    int i;
    for (i = 0; i < n; i++)
        if (s1[i] < s2[i])
            return -1;
        else if (s1[i] > s2[i])
            return 1;
    return 0;
}

void naive_memset(void *p, char c, int n)
{
    unsigned char *s = p;
    int i;
    for (i = 0; i < n; i++)
        s[i] = c;
}








int program_main();

#define FILLCHAR 'E'

int main()
{
    naive_memset(output_buffer, FILLCHAR, sizeof(output_buffer));    
    program_main();
    finish_output();
    nemu_assert(naive_memcmp(output_buffer, answer_buffer, sizeof(answer_buffer)) == 0);
    nemu_assert(memcmp(output_buffer, answer_buffer, sizeof(answer_buffer)) == 0);
    HIT_GOOD_TRAP;
    return 0;
}

#define main program_main
#define scanf naive_scanf
#define printf naive_printf
#define puts(str) naive_printf("%s\n", (str))
#define putchar(ch) naive_printf("%c", (ch))

/* REAL USER PROGRAM */

#include <string.h>
//#include <assert.h>

typedef unsigned long long ULL;
typedef long long LL;

void ULLdivULL(ULL a, ULL b, ULL *qp, ULL *rp)
{
    ULL r = 0, q = 0;
    int i;
    for (i = 0; i < 64; i++) {
        r = (r << 1) + (a >> 63);
        a <<= 1;
        q <<= 1;
        if (r >= b) {
            r -= b;
            q += 1;
        }
    }
    if (qp) *qp = q;
    if (rp) *rp = r;
}

void LLdivLL(LL a, LL b, LL *qp, LL *rp)
{
    int qf = 0, rf = 0;
    if (a < 0) { qf = rf = 1; a = -a; }
    if (b < 0) { qf ^= 1; b = -b; }

    ULLdivULL(a, b, (ULL *) qp, (ULL *) rp);

    if (qp && qf) *qp = -*qp;
    if (rp && rf) *rp = -*rp;
}

#define MAXR 8
#define MOD 1000000007
#define MAXMAT (MAXR + 1)
struct mat {
    int n, m;
    long long a[MAXMAT][MAXMAT];
};

void print_mat(struct mat *a)
{
    int i, j;
    printf("matrix: %d * %d\n", a->n, a->m);
    for (i = 0; i < a->n; i++) {
        for (j = 0; j < a->m; j++)
            printf("%d ", (int) a->a[i][j]);
        printf("\n");
    }
}

void mat_mul(struct mat *c, struct mat *a, struct mat *b)
{
    static struct mat tmp;
    nemu_assert(a->m == b->n);
    tmp.n = a->n;
    tmp.m = b->m;
    int i, j, k;
    for (i = 0; i < tmp.n; i++)
        for (j = 0; j < tmp.m; j++) {
            tmp.a[i][j] = 0;
            for (k = 0; k < a->m; k++) {
                tmp.a[i][j] += a->a[i][k] * b->a[k][j];
                LLdivLL(tmp.a[i][j], MOD, NULL, &tmp.a[i][j]);
            }
        }
    *c = tmp;
}

void mat_pow(struct mat *b, struct mat *a, int n)
{
    nemu_assert(a != b);
    if (n == 1) {
        *b = *a;
        return;
    }
    mat_pow(b, a, n / 2);
    mat_mul(b, b, b);
    if (n % 2)
        mat_mul(b, b, a);
}

int S[MAXR + 1];
int A[MAXR + 1];

void solve()
{
    int i;
    int N, R, K;
    scanf("%d%d%d", &N, &R, &K);
    for (i = 1; i <= R; i++)
        scanf("%d", &S[i]);
    for (i = 1; i <= R; i++)
        scanf("%d", &A[i]);

    static struct mat a, b, c, s, r;
    memset(&a, 0, sizeof(a));
    memset(&s, 0, sizeof(s));
    a.n = a.m = R + 1;
    for (i = 1; i <= R; i++)
        a.a[i][1] = A[i];
    for (i = 1; i < R; i++)
        a.a[i][i + 1] = 1;

    if (N * K <= R) {
        
    }
    /*print_mat(&a);*/

    mat_pow(&b, &a, K);
    b.a[0][0] = b.a[1][0] = 1;
    /*print_mat(&b);*/

    
    s.n = 1, s.m = R + 1;
    for (i = 1; i <= R; i++)
        s.a[0][i] = S[R - i + 1];    
    int scnt = 0;
    int sadd = 0;
    for (i = K; i < R; i += K) {
        sadd += S[i];
        sadd %= MOD;
        scnt++;
        if (scnt == N) {
            printf("%d\n", (int) sadd);
            return;
        }
    }
    for (i = R; i < (scnt + 1) * K; i++)
        mat_mul(&s, &s, &a);
    s.a[0][0] = sadd;
    
    /*print_mat(&s);*/

    mat_pow(&c, &b, N - scnt);
    /*print_mat(&c);*/

    mat_mul(&r, &s, &c);
    /*print_mat(&r);*/

    printf("%d\n", (int) r.a[0][0]);
}

int main()
{
    int T;
    scanf("%d", &T);
    while (T--) solve();
    return 0;
}
