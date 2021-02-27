#include <stdio.h>
#include <string.h>
#include "types.h"
#include "common.h"         //importing user created libraries from the current directory
#include "decode.h"


/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if ( argv[2] == NULL )          //checking if argv2 is passed or not
    {
        return e_failure;
    }

    else
    {
        if ( strstr(argv[2], ".") == NULL )             //if file extension is not passed
        {
            printf("ERROR: Image file extension not detected. Please use the file extension after filename\n");
            return e_failure;
        }

        char *argv2_extn = strstr(argv[2], ".");

        if ( strcmp(".bmp", argv2_extn) == 0 )          //checking if file extension is .bmp
        {
            decInfo->stego_image_fname = argv[2];
        }

        else
        {
            printf("ERROR: Passed file is not a .bmp file\n");
            return e_failure;
        }
    }

    if ( argv[3] == NULL )              //checking if optional argument is passed or not
    {
        decInfo->secret_fname = "DECODED_FILE.txt";
        return e_success;
    }

    else
    {
        decInfo->secret_fname = argv[3];        //set if optional argument is used
        if ( strstr(argv[3], ".") == NULL )     //if extension not passed
        {
            printf("ERROR: Please pass secret file extension\n");
            return e_failure;
        }
        
        if ( strcmp(".txt", strstr(argv[3], ".")) != 0 )        //check if the extension is .txt or not
        {
            printf("ERROR: Please enter a .txt file as output file only\n");
            return e_failure;
        }

        //strcpy(decInfo->extn_file, strstr(argv[3], "."));
        return e_success;
    }
}

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo)
{
    if ( open_files_for_decoding(decInfo) == e_success )        //function call to check if files are opened or not
    {
        printf("INFO: Open files success\n");
        fseek(decInfo->fptr_stego_image, 54, SEEK_SET);         //move the file pointer to 54th position to decode magic string
        
        if ( decode_magic_string(MAGIC_STRING, decInfo) == e_success )  //function call to decode the magic string
        {
            printf("INFO: Input image is an encoded image\n");
            printf("INFO: Magic string decode successful\n");
            
            if ( decode_secret_file_extn_size(decInfo) == e_success )   //function call to decode file extension size
            {
                printf("INFO: File extension size decoded successfully\n");

                if ( decode_secret_file_extn(decInfo->extn_file, decInfo) == e_success )    //function call to decode file extension
                {
                    printf("INFO: Secret file extension decoded successfully\n");

                    if ( decode_secret_file_size(decInfo->size_secret_file, decInfo) == e_success ) //function call to decode secret file size
                    {
                        printf("INFO: Secret file size decoded successfully\n");

                        if ( decode_secret_file_data(decInfo) == e_success )        //function call to decode data and store it in file
                        {
                            printf("INFO: Secret file data decoded successfully\n");
                            printf("INFO: Decoded secret file created successfully in the current directory\n");
                            return e_success;           //return to main program
                        }

                        else
                        {
                            printf("ERROR: Secret file data decode failure\n");
                            return e_failure;
                        }
                    }

                    else
                    {
                        printf("ERROR: Secret file size decoding failure\n");
                        return e_failure;
                    }
                }

                else
                {
                    printf("ERROR: Secret file extension decoding failure\n");
                    return e_failure;
                }
            }
            else
            {
                printf("ERROR: File extension size decoding failure\n");
                return e_failure;
            }
        }

        else
        {
            printf("ERROR: Input image is not an encoded image");
            return e_failure;
        }
    }
    else
    {
        printf("ERROR: Open files failure\n");
        return e_failure;
    }
}

/* Get File pointers for i/p and o/p files */
Status open_files_for_decoding(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");     //opening the passed image file in read mode

    if ( decInfo->fptr_stego_image == NULL )        //file pointer is not NULL that is to check if it exists  ir not
    {
        perror("fopen");                            //perror prints the error message in descriptive form to stderr
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }

    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");       //open secret file in write mode

    if ( decInfo->fptr_secret == NULL )
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);
        return e_failure;
    }

    return e_success;
}

/* Store Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char buffer[8];
    int count = 0;      //count to check the matched LSB characters with # and *

    for ( int i = 0; i < 2; i++ )   //iterate the loop 2 times for # and *
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_image);     //take 8 bytes from stego image to buffer
        
        for ( int j = 0; j < 8; j++ )
        {
            if ( (buffer[j] & 1) == !!(magic_string[i] & (1 << (7 - j))) )      //Checking if LSB of buffer matches with MSB to LSB of # and *
            {
                count += 1;         //increase the count if matched
            }
        }
    }

    if ( count == 16 )              //if count is 16 then it is an encoded image
    {
        return e_success;
    }

    else                            //if not matched it is not an encoded image
    {
        return e_failure;
    }
}

/* Decode secret file extenstion size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
    int extn_size = 0;

    fread(buffer, 1, 32, decInfo->fptr_stego_image);        //pass 32 bytes of file to buffer

    for ( int i = 0; i < 32; i++ )
    {
        extn_size |= (buffer[i] & 1) << (31 - i);           //Take to LSB bits of the buffer and store it in MSB to LSB bits of size bit by bit
    }

    decInfo->size_file_extn = extn_size;            //putting the value of file_extn in structure variable
    
    return e_success;
}

/* Decode secret file extenstion */
Status decode_secret_file_extn(const char *file_extn, DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;
    char extn[4];       

    for ( int i = 0; i < decInfo->size_file_extn; i++ )
    {
        ch = 0;
        fread(buffer, 1, 8, decInfo->fptr_stego_image);     //get 8 bytes from the file
        
        for ( int j = 0; j < 8; j++ )
        {
            ch |= (buffer[j] & 1) << (7 - j);               //adding LSB bits of buffer one by one to MSB to LSB bits of ch variable
        }
        extn[i] = ch;                                       //put the ch in extn array
    }

    strcpy(decInfo->extn_file, extn);           //copy the file extension to a structure variable
    return e_success;
}


/* Decode secret file size */
Status decode_secret_file_size(uint file_size, DecodeInfo *decInfo)
{
    file_size = 0;
    
    char buffer[32];            //32 byte buffer to find integer bytes of the size

    fread(buffer, 1, 32, decInfo->fptr_stego_image);    //get 32 bytes from the image buffer

    for ( int i = 0; i < 32; i++ )                      //iterate loop 32 times as size of int is 4 bytes so 32 bits
    {
        file_size |= (buffer[i] & 1) << (31 - i);       //get byte by byte from LSB of buffer and store it in MSB to LSB of file size var
    }

    decInfo->size_secret_file = file_size;              //store file size in structure variable

    return e_success;
}


/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8], ch;         //creating buffer of size 8 array, as each char is of 1 byte or 8 bits

    for ( int i = 0; i < decInfo->size_secret_file; i++ )       //iterate as per the size of the secret file
    {
        ch = 0;                 //turn the character to 0 every time the loop runs
        fread(buffer, 1, 8, decInfo->fptr_stego_image);         //read 8 bytes from the image and store it in buffer
        for ( int j = 0; j < 8; j++ )                           //iterate as per the characeter size
        {
            ch |= (buffer[j] & 1) << (7 - j);                   //get the character from 8 bytes buffer LSB and to get all 8 bits of character
        }
        fprintf(decInfo->fptr_secret, "%c", ch);                //store the character in the file one by one
    }
    return e_success;
}

