#include <stdio.h>
#include <time.h>

#define FILTER_N 10

int i, j;
int filter_temp, filter_sum = 0;
int filter_buf[FILTER_N] = {1,3,6,5,4,8,0,1,2};
clock_t start, end;

int main()
{
    start = clock();
    for (i = 1; i < FILTER_N; i++)
    {
        for (j = i; j > 0; j--)
        {
            if (filter_buf[j] < filter_buf[j - 1])
            {
                //内层循环从i开始，依次与见面的数进行比较，一直比较到第一个大于当前元素为止
                filter_temp = filter_buf[j];
                filter_buf[j] = filter_buf[j - 1];
                filter_buf[j - 1] = filter_temp;
            }else break;
        }
    }
    end = clock();
    for (i = 1; i < FILTER_N; i++)
    {
        printf("%d  ", filter_buf[i]);
    }

    printf("\n");
    printf("耗时:%f", end - start);
}
