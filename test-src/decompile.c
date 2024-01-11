#include <stdio.h>
#include <stdlib.h>

//unsigned int main ( unsigned int lim )
unsigned int main ( int argc, char *argv[] )
{
    int result = 0;
    int lim = argv[1] - '0';
    printf("%d\n", lim);
    int i;

    for(i = 0; i < lim; i++) {
      result = result << 2;
    }

    printf("%d\n", result);

    return result;
}
