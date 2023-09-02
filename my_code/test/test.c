#include<stdio.h>
#include<string.h>

int main()
{

    unsigned char a[4];
    a[0]=0xFE;
    a[1]=0xFF;
    a[2]=0xFF;
    a[3]=0xFF;

    float b [1];
    memcpy(b,a,4);
    printf("%f", b[0]);
    return 0;
}