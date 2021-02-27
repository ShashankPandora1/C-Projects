#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define MAGIC_STRING "#*"

int main(int argc, char **argv)
{
    FILE *fptr;

    fptr = fopen(argv[1], "r");

    fseek(fptr, 54, SEEK_SET);
    int  i, j, count = 0;
    char buffer[8];

    for ( i = 0; i < 2; i++ )
    {

        fread(buffer, 1, 8, fptr);

        for ( j = 0; j < 8; j++ )
        {
            if ( (buffer[j] & 1) == !!(MAGIC_STRING[i] & (1 << (7 - j))) )
            {
                count += 1;
            }
        }
    }

    if ( count == 16 )
    {
        printf("THis is an encoded image\n");
    }

    else
        printf("THis is not an encoded image\n");

    int size = 0;
    char buf_32[32];

    fread(buf_32, 1, 32, fptr);

    for ( i = 0; i < 32; i++ )
    {
        size |= (buf_32[i] & 1) << (31 - i);
    }

    printf("%d\n", size);

    char buf_8[8];
    char ch = 0;
    char extn[5];

    for ( i = 0; i < 4; i++ )
    {
        ch = 0;
        fread(buf_8, 1, 8, fptr);

        for ( int j = 0; j < 8; j++ )
        {
            ch |= (buf_8[j] & 1) << (7 - j);
        }
        printf("%c", ch);
        extn[i] = ch;
    }

    if ( strcmp(extn, ".txt") == 0 )
        printf("\nExtension decode successful\n");
    else
        printf("\nFailed\n");

    fread(buf_32, 1, 32, fptr);
    size = 0;

    for ( i = 0; i < 32; i++ )
    {
        size |= (buf_32[i] & 1) << (31 - i);
    }

    printf("\nsize: %d\n", size);

    for ( i = 0; i < 25; i++ )
    {
        fread(buf_8, 1, 8, fptr);
        ch = 0;

        for ( j = 0; j < 8; j++ )
        {
            ch |= (buf_8[j] & 1) << (7 - j);
        }
        printf("%c", ch);
    }

    printf("\n");


    /*
    fread(buf_8, 1, 8, fptr);
    ch = 0;

    for ( i = 0; i < 8; i++ )
    {
        ch |= (buf_8[i] & 1) << (7 - i);
    }

    printf("%c\n", ch);
*/

    return 0;
}

