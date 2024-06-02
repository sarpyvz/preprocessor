#include "P_HEADER.h"
#include <stdio.h>

int main()
{

    @int A(10);
    printf("\n");
    @init A = 3;
    printf("\n");
    @print A;
    printf("\n");
    @int B(20);
    printf("\n");
    @init B = 5;
    printf("\n");
    @print B;
    printf("\n");
    @add C = A + B;
    printf("\n");
    @print C;
    printf("\n");
    @int X(8,5);
    @init X = 3;
    printf("\n");
    @print X;
    printf("\n");
    @int Y(5,4);
    @init Y = 6;
    printf("\n");
    @print Y;
    printf("\n");
    @mmult Z = X * Y;
    printf("\n");
    @print Z;
    printf("\n");
    @aver B
    printf("\n");
    @sum A
    return 0;
}
