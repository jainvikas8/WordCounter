# WordCounter

This program is a console application that creates a dictionary of words found in a text file and prints the dictionary 
together with the frequency of each unique word found

The case or punctuation is ignored and the output is not sorted.

It accepts the file name as a parameter via command line run.

RULES:

    * Any letter A-Z or a-z is converted to lower case
    * Occurrence of alphabet defines the start of the word and a non-alphabet defines the end of the word.
    * Any numbers or special characters are not considered to be a part of a word
    * Word recognized by the program doesn't validate that it has a meaning in a real world.
    * Larger the file more RAM requirement will be needed

>NOTE : Big text samples have been taken from http://www.gutenberg.org a free ebook site. This is only for testing purpose.

Compile :  gcc -o WordCounter WordCounter.c

Execute : ./WordCounter <*filename*>          

