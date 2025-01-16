# Implementing-SDES-and-analyzing-Encryption-Decryption-modes-for-Images-
This project is about analyzing pictures in BMP format in various mode like EBC, CBC and CTR.

Credit: 
This file reading of the BMP image in main.c is credited to Nathaniel's Dobb;s Image Processing Code.

********************************************************

FILES INCLUDED:
main.c - main driver program that reads from a bmp image (normal.bmp the defaul), asks which form of encryption to use (ECB, CBC, or CTR), and calls the encryption funtion or decryption funtion (encryption default). Then writes the encrypted text to specified file or cypher.bmp(default)

SDES.c - program that contains all the backend logic and funcitonality of the encryption/decryption. This encludes key generation, swapping, substitution, and permuation

SDES.h - header file that contains all the function declarations and external variables

********************************************************
HOW TO COMPILE:
Using gcc from the command line, the project can be compiled as followed: (same as from file given by Nathanial Dobbs image processsing code)

example: 
C:\...filepath...\FolderOfContents> gcc -o sdes main.c SDES.c
C:\...filepath...\FolderOfContents> ./sdes

********************************************************

NOTES:
-All files must be in the same folder and file path for them to correctly function with one another
-There must be a normal.bmp file or another specified bmp file for the program to correctly function
-Cypher.bmp will be overwritten if the image or key are updated and the program is ran again


