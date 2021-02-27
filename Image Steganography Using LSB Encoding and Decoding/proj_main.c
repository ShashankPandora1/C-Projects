#include<stdio.h>
#include"encode.c"
#include"decode.c"
#include"common.h"
int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    int type, state;
    type = check_operation_type(argv);
    if (type == e_encode)
    {
        state = read_and_validate_encode_args(argv, &encInfo);
        if (state == e_success)
        {
            printf("INFO: Validation is success");
            state = do_encoding(&encInfo);
            if(state == e_success)
            {
                printf("INFO :Encoding is done Successfully\n");
            }
            else
            {
                printf("ERROR: Encode Failure");
                return -1;
            }
        }
        else
        {
            printf("./lsb_steg -e File_name.bmp    secret.txt <output.bmp>\n");
            return -1;
        }

    }
    
       else if (type == e_decode)
       {
           state = read_and_validate_decode_args(argv, &decInfo);
           if (state == e_success)
           {
               printf("\ndecoding\n");
               state = do_decoding(&decInfo);
               if(state == e_success)
               {
                   printf("decoding success\n");
               }
               else
               {
                   printf("decoding failed\n");
                   return -1;
               }
           }

       }
     
    else
    {
        printf("Unsupported\n./lsb_steg -e File_name.bmp    secret.txt <output.bmp>\n./lsb_steg -d File_name.bmp    <decode.txt>\n");
    }
    return 0;
}
