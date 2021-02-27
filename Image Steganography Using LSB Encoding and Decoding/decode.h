#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
	 /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
	
    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    
   	char magic_string[3];
	int size_file_extn;
	char extn_file[MAX_FILE_SUFFIX];
	long size_secret_file;
	char secret_data[MAX_SECRET_BUF_SIZE];
	
} DecodeInfo;

/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_for_decoding(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(const char *file_extn, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(uint file_size, DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode function, which does the real encoding */
Status Decode_data_to_file(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encoding  extention size and file size */
Status Decode_size(int size, FILE *fptr_src_image);

/* Decode size to lsb */
Status decode_size_to_LSB(int size, char *buffer);

/* Get file size */
uint get_file_size(FILE *fptr);
#endif
