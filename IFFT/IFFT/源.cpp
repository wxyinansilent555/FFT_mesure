#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define PI 3.14159265358979

clock_t start, stop;
double duration;

typedef struct Complex
{
    double real;
    double image;
} CPLX;

void _fft(CPLX buf[], CPLX out[], int n, int step)
{
    int i;
    CPLX t;
    double a, b, c, d, theta;

    if (step < n)
    {
        _fft(out, buf, n, step * 2);
        _fft(out + step, buf + step, n, step * 2);

        for (i = 0; i < n; i += 2 * step)
        {
            theta = -PI * i / n;
            a = cos(theta);
            b = sin(theta);
            c = out[i + step].real;
            d = out[i + step].image;
            t.real = a * c - b * d;
            t.image = b * c + a * d;
            buf[i / 2].real = out[i].real + t.real;
            buf[i / 2].image = out[i].image + t.image;
            buf[(i + n) / 2].real = out[i].real - t.real;
            buf[(i + n) / 2].image = out[i].image - t.image;
        }
    }
}

int fft(CPLX buf[], int n)
{
    int i;
    CPLX* out = (CPLX*)malloc(n * sizeof(CPLX)); //malloc（n）分配一个n字节的内存，并把指针赋给out，
    if (out == NULL)                             //如果不能回应分配内存，则返回一个NULL
    {
        return 0;
    }

    for (i = 0; i < n; i++)
    {
        out[i].real = buf[i].real;
        out[i].image = buf[i].image;
    }

    _fft(buf, out, n, 1);

    free(out);
    out = NULL;
    return 1;
}

int main()
{
    start = clock();
    #define COUNT 8
    CPLX buf[COUNT] = { {1.0, 0.0}, {1.0, 0.0}, {1.0, 0.0}, {1.0, 0.0},
                     {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0} ,{0.0, 0.0} }; //实向量[1, 1, 1, 1, 0, 0, 0, 0]
    fft(buf, COUNT);

    for (int i = 0; i < COUNT; i++)
    {
        if (buf[i].image >= 0.0)
        {
            printf("%.4f + %.4fi\n", buf[i].real, buf[i].image);
        }
        else
        {
            printf("%.4f - %.4fi\n", buf[i].real, fabs(buf[i].image));
        }
    }
    stop = clock();
    duration = ((double)(stop - start)) / CLK_TCK;
    printf("执行时间：%f", duration);
    return 0;

}
