#include <stdio.h>
#include <string.h>             //to use sring functions like strcmp, strcpy
#include "encode.h"
#include "common.h"             //getting information of all the user defined libraries from current directory
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("INFO: Image Resolution: %u x %u\n", width, height);

    rewind(fptr_image);

    // Return image capacity
    return width * height * 3;
}

/* Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");         //opening source image file in read mode
    
    if (encInfo->fptr_src_image == NULL)        //fopen returns NULL if file passed doesn't exist
    {
    	perror("fopen");                        //perror prints the descriptive error message to stderr
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);   //print error message to stderr
    	return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");   //opening secret file in read mode
    
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");     //opening stego file in write mode
    
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
    	return e_failure;
    }

    return e_success;
}

/* Check operation type */
OperationType check_operation_type(char *argv[])
{
    if ( argv[1] == NULL )      //if no argument passed as argv[1] go to this block
    {
        return e_failure;
    }

    if ( strcmp(argv[1], "-e") == 0 )   //if -e passed go to this block
    {
        return e_encode;
    }

    else if ( strcmp(argv[1], "-d") == 0 )
    {
        return e_decode;
    }

    else                        //if arguments other than -e and -d passed
    {
        return e_unsupported;
    }
}

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if ( argv[2] == NULL )      //if argument 2 is not passed
    {
        return e_failure;
    }

    else
    {
        if ( strstr(argv[2], ".") == NULL )     //if only file name passed but not the extension
        {
            printf("ERROR: Image file extension not detected. Please use the file extension after the file name\n");
            return e_failure;
        }
        char *argv2_extn = strstr(argv[2], ".");    //extract the extension from the filename and and store it in argv2 address
        
        if ( strcmp(".bmp", argv2_extn) == 0 )      //checking if it is a .bmp file or not
        {
            encInfo->src_image_fname = argv[2];
        }

        else        //goto this block to print the error message that passed file is not a .bmp file
        {
            printf("ERROR: Passed file is not a .bmp extension file\n");  
            return e_failure;
        }

    }

    if ( argv[3] == NULL )          //go to this block if argument 3 is not passed
    {
        printf("ERROR: Please pass the secret file\n");
        return e_failure;
    }

    else
    {
        if ( strstr(argv[3], ".") == NULL )     //checking for the file extension
        {
            printf("ERROR: Secret file extension not detected. Please use the file extension after file name\n");
            return e_failure;
        }
        char *argv3_extn = strstr(argv[3], ".");

        if ( strcmp(".txt", argv3_extn) == 0 )  //checking if extension is .txt
        {
            encInfo->secret_fname = argv[3];
            strcpy(encInfo->extn_secret_file, ".txt");      //copy the file extension to a structure variable
        }

        else
        {
            printf("ERROR: Passed file is not a .txt file\n");
            return e_failure;
        }
    }

    if ( argv[4] == NULL )          //output encoded image arg 4 check
    {
        encInfo->stego_image_fname = "stego.bmp";
        return e_success;
    }

    else
    {
        if ( strstr(argv[4], ".") == NULL )
        {
            printf("ERROR: Please pass output image file extension\n");
            return e_failure;
        }
        
        if ( strcmp(".bmp", strstr(argv[4], ".")) != 0 )
        {
            printf("ERROR: Please pass .bmp file only as output file\n");
            return e_failure;
        }
        encInfo->stego_image_fname = argv[4];
        return e_success;
    }
}

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo)
{
    if ( open_files(encInfo) == e_success )         //function call to open the files passed through CLA
    {
        printf("INFO: Open files success\n");
        
        if ( check_capacity(encInfo) == e_success )         //checking if image size is greater than secret file
        {
            printf("INFO: Check capacity success\n");
            
            if ( copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success )     //function call to copy header
            {
                printf("INFO: Copy BMP header success\n");
                
                if ( encode_magic_string(MAGIC_STRING, encInfo) == e_success )      //function call to encode magic string bytes
                {
                    printf("INFO: Magic String encode success\n");
                    
                    if ( encode_size(strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success )
                    {                           //function call to encode file extension size
                        printf("INFO: Extension size encode success\n");
                        
                        if ( encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success )
                        {                       //function call to encode secret file extension
                            printf("INFO: Secret file extension encode success\n");
                            
                            if ( encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success )
                            {                   //function call to encode the size of the secret file
                                printf("INFO: Secret file size encode success\n");
                                
                                if ( encode_secret_file_data(encInfo) == e_success )
                                {               //function call to encode the secret file data
                                    printf("INFO: Secret file data encode success\n");
                                    
                                    if ( copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success )
                                    {           //copying rest of the bytes to image
                                        printf("INFO: Copy remaining image data success\n");
                                        fclose(encInfo->fptr_src_image);
                                        fclose(encInfo->fptr_secret);           //close all the files
                                        fclose(encInfo->fptr_stego_image);
                                        return e_success;                       //return to main program after finished decoding
                                    }
                                    else        //print error message if data copying is unsuccessful
                                    {
                                        printf("ERROR: Copying remaining image data unsuccessful\n");
                                        return e_failure;                       //return to main program and passing value of enum var
                                    }
                                }
                                else
                                {
                                    printf("ERROR: Secret file data encoding failure\n");
                                    return e_failure;
                                }        
                            }
                            else
                            {
                                printf("ERROR: File Size encoding failure\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("ERROR: Secret file extension encoding failure\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("ERROR: Extension size encoding is failure\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("ERROR: Magic String encode is failure\n");
                    return e_failure;
                }
            }
            else
            {
                printf("ERROR: Copy BMP header failure\n");
                return e_failure;
            }
        }
        else
        {
            printf("ERROR: Cannot do encoding because Image capacity is less");
            return e_failure;
        }
    }
    else
    {
        printf("ERROR: Open Files Failure\n");
        return e_failure;
    }   
}


/* check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);      //function call to get the size if the image and store
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);    //store the size of the secret file

    if ( encInfo->image_capacity >= (encInfo->size_secret_file * 8 + 54 + 16 + 32 + 32) )  //function to check the capacity
    {                       //file size * 8 as each character is 1 byte, 54 for image header; 32 for extn size, file 4*8(32) to encode extn
        return e_success;   //16 for magic string encode                                                     //characters, 32 for file size
    }
    else
    {
        return e_failure;
    }
}

/* Get file size */
uint get_file_size(FILE *fptr)
{
    uint size;                      //declaring size variable of unsigned int data type
    fseek(fptr, 0, SEEK_END);       //move the file pointer location from 0th position to the end
    size = ftell(fptr);             //store the file pointer location in size
    rewind(fptr);                   //go back to 0th position in the file
    return size;                    //return file size
}

/* Copy bmp image header */
Status copy_bmp_header(FILE * fptr_src_image, FILE *fptr_dest_image)
{
    char header[54];                //declarating character array of 54 bytes to store header info
    rewind(fptr_src_image);
    fread(header, 1, 54, fptr_src_image);   //copy header 54 bytes of source image to header array
    fwrite(header, 1, 54, fptr_dest_image); //store the header in first 54 bytes of new image.bmp file
    return e_success;
}

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;                   //function call to encode magic string
}

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;                   //function call to encode secret file extension
}

/* Encode secret file size */
Status encode_secret_file_size(uint file_size, EncodeInfo *encInfo)
{
    encode_size(file_size, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;                   //function call to encode size
}

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)     //function definition to encode secret file data
{
    for ( int i = 0; i < encInfo->size_secret_file; i++ )   //iterate loop as per the size of the secret file
    {
        fread(encInfo->secret_data, 1, 1, encInfo->fptr_secret);    //copy one byte from the secret file and store in buffer
        fread(encInfo->image_data, 1, 8, encInfo->fptr_src_image);  //get 8 bytes from source image and store it in buffer
        encode_byte_tolsb(*encInfo->secret_data, encInfo->image_data);  //to encode 1 byte of data into LSB of the image
        fwrite(encInfo->image_data, 1, 8, encInfo->fptr_stego_image);   //write the updated buffer data to new image
    }
    return e_success;
}

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    //to encode 2 bytes we need 2 * 8 = 16 bytes
    for ( int i = 0; i < size; i++ )            //iterate the loop as per the passed size
    {
        //read 8 bytes from fptr source image store it in buffer image_data buffer
        fread(buffer, 1, 8, fptr_src_image);    //read 8 bytes from the source image and store it in buffer
        encode_byte_tolsb(data[i], buffer);     //encode the data into 8 bytes LSB
        fwrite(buffer, 1, 8, fptr_stego_image); //store updated buffer in new image
    }
    return e_success;
}

/* Encode a byte into LSB of image data array */
Status encode_byte_tolsb(char data, char *image_buffer)
{
    for ( int i = 0; i < 8; i++ )               //iterate 8 times
    {                                           //update the LSB of passed buffer array with data bits
        image_buffer[i] = (image_buffer[i] & ~1) | !!(data & (1 << (7 - i)));   //clearing LSB bits of bufffer array and storing data bits
    }
    return e_success;
}

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    //read from fptr source
    while ( fread(&ch, 1, 1, fptr_src) )        //read 1 byte from source file until the end is reached if '\0' or NULL is detected
    {
        fwrite(&ch, 1, 1, fptr_dest);           //write the byte to the new image file
    }
    return e_success;
}

//encoding the extension size and file size
Status encode_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[32];
    fread(buffer, 1, 32, fptr_src_image);       //get 32 bytes from the image and store in buffer

    encode_size_LSB(size, buffer);              //pass the size and buffer base address to encode
    fwrite(buffer, 1, 32, fptr_stego_image);    //write the updated buffer to a new file
    return e_success;
    
}

//encode size to lsb
Status encode_size_LSB(int size, char *buffer)
{
    for ( int i = 0; i < 32; i++ )              //encode all 32 bits of size from MSB to LSB in the LSB bits of buffer
    {
        buffer[i] = (buffer[i] & ~1) | !!(size & (1 << (31 - i)));      //update LSB bits of buffer
    }
    return e_success;
}
