#include  <stdio.h>//for printf
#include  <stdlib.h>//for exit
int test();
int main(int argc, char **argv)
{
        int i = 1;
        i+=2;

        printf("Hello, world (i=%d)!\n", i);
        printf("test(i=%d)!\n", test());
        return 0;
}
