/*
	Author : Vikas Katariya
	License : Free 

	This program is a console application that creates a dictionary of words found in a text file and prints the dictionary 
	together with the frequency of each unique word found

	The case or punctuation is ignored and the output is not sorted.

	It accepts the file name as a paramter via command line run.	

	RULES:
		*	Any letter A-Z or a-z is converted to lower case
		* 	Occurrence of alphabet defines the start of the word and a non-alphabet defines the end of the word.
		*	Any numbers or special characters are not considered to be a part of a word
		* 	Word recognised by the program doesn't validate that it has a meaning in a real world.
		* 	Larger the file more RAM requirement will be needed
		*   Maximum length of the word is (WORD_MAX_SIZE - 1), if bigger than this then it will be truncated

	Example OUTPUT:
		--------Word Counter----------

		Words starting with 't'

				testing  -- 4
				today  -- 1
				the  -- 2
				totally  -- 2
				to  -- 1

		Words starting with 'a'

				a  -- 1
				alpha  -- 1

		--------------------------------

		Parsing took about 0.000000 seconds
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define WORD_MAX_SIZE 21	//This is maximum word length allowed to be recorded in dictionary (x-1), last one string terminator 

#define DEBUG
#undef DEBUG

//Words dictionary
typedef struct words_dict
{
	char word[WORD_MAX_SIZE];
	unsigned long count;//How many found?
	unsigned char word_length;//What's the length of the word
	struct words_dict *next;
} words_t;

//Entries - Usually first letter (a-z) 
typedef struct entries_dict 
{
	unsigned char entry_char;
	words_t *subset;//Nested linked list
	struct entries_dict *next;//Next node
} dict_t;

/*	Function : Check if the character is alphabet, this also convert a Upper case to lower
	@param : (char *) Alphabet
	@return :	(bool) True if the character is alphabet*/
bool checkAlpha(char *alpha)
{
	bool status = false;

	if ((*alpha >= 'a' && *alpha <= 'z')
			|| 
			(*alpha >= 'A' && *alpha <= 'Z'))
	{
		*alpha |= 0x20;//Convert upper to lower, if already lower case it has not effect
		status = true;
	}

	return status;
}

/*	Function : Print the dictionary
	@param : (dict *) Start of the dictionary
	@return :	none*/
void printDictionaryCount(dict_t *head)
{
    dict_t *current = head;
	unsigned long totalWords = 0;

	printf("\n--------Word Counter----------\n\n");

    while (current != NULL) 
	{
		words_t *curWord = current->subset;
	
		printf("Words starting with '%c'\n", current->entry_char);

		while (curWord != NULL)
		{
        	printf("\n\t%s  -- %ld", curWord->word, curWord->count);

			totalWords += curWord->count;

			curWord = curWord->next;
		}

		printf("\n\n");

        current = current->next;
    }

	printf("\n Total number of words : %ld", totalWords);

	printf("\n--------------------------------\n");
}

/*	Function : Start reading the characters from the file and build a dictionary of found words
	@param : (FILE *) file
	@return :	none*/
void buildDict(FILE *readFile)
{
	unsigned char curIndex = 0;//Temporary buffer counter
	char readChar;
	char tempWord[WORD_MAX_SIZE] = {0};
	bool isAlpha = false;
	dict_t *head = NULL;

#ifdef DEBUG

	printf("--------DEBUG PRINT--------");

#endif //DEBUG

	while ((readChar = fgetc(readFile)) != EOF //Start reading character by character until end of file
			||
			curIndex > 0) //Or if we have reached EOF, see if we have something in the buffer?
	{	
		isAlpha = checkAlpha(&readChar);

		if(isAlpha == false && curIndex > 0) //Check if we have non alphabet and have something in temporary buffer
		{
			bool wordFound = false;

			tempWord[curIndex++] = '\0'; //Add the string terminator

#ifdef DEBUG

			printf("\nCurrent word : %s", tempWord);

#endif //DEBUG

			dict_t *current = head;//Lets take a working copy

			//Scan for duplicate entry
			while (current != NULL && wordFound == false)//Valid pointer and we should have not found what we where looking for
			{
				if(tempWord[0] == current->entry_char)//If it matches the first letter, then search the subset
				{
					words_t *newWord = current->subset;

					while(newWord != NULL) //Search each word
					{
						if(newWord->word_length == curIndex //We know the length of the word, just quickly find it and compare further if necessary
							&& 	
							strcmp(&newWord->word[0], &tempWord[0]) == 0)
						{
							printf("\t\tSimilar word found");
							newWord->count++;//Increment
							wordFound = true; //start the scan again

							break; //We are done
						}

						if(newWord->next != NULL)//We move to next entry if its present or else we might have to add one...
						{
							newWord = newWord->next;
						}
						else
						{
							break;
						}
					}

					if (wordFound == false)//If not yet found, add a new word to the subset
					{
						words_t *newSubset = NULL;

						newSubset = (words_t *)malloc(sizeof(words_t));

#ifdef DEBUG

						printf("\t\tAdding new entry to the subset");

#endif //DEBUG

						if(newSubset == NULL)
						{
							printf("\nCan't append a new word");
							exit(-1);
						}

						newSubset->word_length = curIndex;	//Length of the word
						newSubset->count = 1;			//How many counts we have for this word

						memcpy(&newSubset->word[0], &tempWord[0], curIndex);
						newSubset->next = NULL;

						wordFound = true; //start the scan again

						if(current->subset == NULL) //Assign for the first time
						{
							current->subset = newSubset;
						}
						else
						{
							// We should be at the end of the linked list here, so just add, save time
							newWord->next = newSubset;

							/*words_t *parse = current->subset;

							while (parse->next != NULL) 
							{
								parse = parse->next;
							}

							parse->next = newSubset;// now we can add a new variable */
						}

						break;
					}
				}
				
				if(current->next != NULL) //We move to next entry if its present or else we might have to add one...
				{
					current = current->next;
				}
				else
				{
					break;//We have reached the end of the list
				}
			}

			if(wordFound == false) //Still no main entry? Add a new one
			{
#ifdef DEBUG

				printf("\t\tAdding new Entry");

#endif //DEBUG

				dict_t *newEntry = NULL;

				newEntry = (dict_t *)malloc(sizeof(dict_t));

				if(newEntry == NULL)
				{
					printf("\nCan't append entries to dictionary");
					exit(-1);
				}
			
				newEntry->entry_char = tempWord[0];	//Get the first character

				newEntry->subset = (words_t *)malloc(sizeof(words_t));

				if(newEntry->subset == NULL)
				{
					printf("\nCan't append entries to dictionary - subset");
					exit(-1);
				}

				newEntry->subset->word_length = curIndex;	//Length of the word
				newEntry->subset->count = 1;			//How many counts we have for this word

				memcpy(&newEntry->subset->word[0], &tempWord[0], curIndex);

				newEntry->subset->next = NULL;
				newEntry->next = NULL;

				if(head == NULL) //Assign it for the first time
				{
					head = newEntry;
				}
				else
				{
					// We should be at the end of the linked list here, so just add, save time
					current->next = newEntry;
					/*dict_t *parse = head;

					while (parse->next != NULL) 
					{
						parse = parse->next;
					}

					parse->next = newEntry; // now we can add a new variable */
				}
			}
			
			curIndex = 0; //reset for new word to be accumulated
		}
		
		if(isAlpha == true && curIndex < (WORD_MAX_SIZE - 1))//Leave last byte for string terminator
		{
			tempWord[curIndex++] = readChar;//Just add to the temp buffer
		}
	}

	printDictionaryCount (head); //Lets print
}

/*	Function : Parse the file for words
	@param : (char *) filename
	@return :	none*/
void parseFile(char *input)
{
	FILE *input_file = fopen(input, "r");//Open as read only

	if (input_file == NULL)
	{
		printf("Sorry, cannot open input file '%s', please refer example below\n", input);
		printf("$./WordCounter example.txt\n");		//fopen returns 0, the NULL pointer, on failure

		exit(0);
	}
	else
	{    
		time_t start, stop;

		time(&start);
		
		buildDict(input_file);

   		time(&stop);
   
   		printf("Parsing took about %.6f seconds. \n", difftime(stop, start));

		fclose(input_file); //Close file
	}
}

/*	Function : main
	@param : (int) number of arguments
	@param : (char **) arguments
	@return :	none*/
void main(int argc, char **argv)
{
	char *input = argv[1]; //Argument as file name

#ifdef DEBUG

	//Test cases
	parseFile("case1.txt");

	parseFile("case2_100k.txt");

	parseFile("case3_1M_plus.txt");

#endif //DEBUG

	parseFile(input);
}