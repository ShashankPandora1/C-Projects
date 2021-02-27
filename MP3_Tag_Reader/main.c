/*
Author Name	:- Shashank Pandora
Organisation	:-Emertxe Information Technologies (P) Ltd.
Date		:-11 July 2020
Description	:-To execute and view and modify mp3 file metadata content from the command line arguments. 
Input		:-User will give the required options and aurguments from CLA and will view or modify the metadata content.
Output		:-For viewing metadata content
		  ./a.out -v sample.mp3
		  For modifying metadata content
		  ./a.out -e sample.mp3 <modifier option> <metadata content>
*/

#include<stdio.h>
#include "view.c"
#include "validate_inputs.c"
#include "common.h"
#include "edit.c"
int main(int argc, char*argv[])
{
	//Declare a variable
	int status;

	//Calling function for validation
	status = validate_inputs(argv);


	if (status == SUCCESS)
	{
		if ((strncmp(*(argv + 1),"-v", 2)) == 0)
		{
			status = view(argv);

			if (status == SUCCESS)
			{
				printf("View SUCCESS\n");
			}
			else
			{
				printf("View ERROR\n");
			}
		}
		else if ((strncmp(*(argv + 1),"-e", 2)) == 0)
		{
			status = edit(argv);
int main(int argc, char*argv[])
{
	//Declare a variable
	int status;

	//Calling function for validation
	status = validate_inputs(argv);


	if (status == SUCCESS)
	{
		if ((strncmp(*(argv + 1),"-v", 2)) == 0)
		{
			status = view(argv);

			if (status == SUCCESS)
			{
				printf("View SUCCESS\n");
			}
			else
			{
				printf("View ERROR\n");
			}
		}
		else if ((strncmp(*(argv + 1),"-e", 2)) == 0)
		{
			status = edit(argv);

			if (status == SUCCESS)
			{
				printf("EDIT SUCCESS\n");
			}
			else
			{
				printf("Edit ERROR\n");
			}
		}
	}
	else if(status == ERROR)
	{
		printf("Validation ERROR\n");
	}
}
			if (status == SUCCESS)
			{
				printf("EDIT SUCCESS\n");
			}
			else
			{
				printf("Edit ERROR\n");
			}
		}
	}
	else if(status == ERROR)
	{
		printf("Validation ERROR\n");
	}
}
