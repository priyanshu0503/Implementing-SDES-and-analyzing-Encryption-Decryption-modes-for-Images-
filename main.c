/*
Submitted by: Priyanshu Mittal and Taitlyn Gowlovech
Date of Submission: 9/23/2024
This file is the driver program for the SDES algorithm that also reads in a BMP image and encrypts it.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "SDES.h"


/*customByte* InitialPerm(customByte* byte);
customByte* Reverse(customByte* byte);
char getChar(customByte* byte);
customByte* functionF(customByte* byte, customByte* key);
customByte* Expansion(customByte* byte);
customByte* XOR(customByte* first, customByte* second);
customByte* SBoxes(customByte* byte);
int conversionToInt(int one, int two);
customByte* conversionToBin(int num);
customByte* P4(customByte* halfByte);
customByte* LRSwaped(customByte* byte, customByte* right);
customByte* XOR4(customByte* first, customByte* second);
customByte* FinalPerm(customByte* byte);
int binaryToInt(customByte* binary);
void printByte(customByte* byte);*/

void parseKey(const char* keyString, int key[])
{
    for (int i = 0; i < 10; i++) {
        key[i] = keyString[i] - '0';
    }
}

int main(int argc, char *argv[])
{

    bool decrypt = false;
    char* fileName = NULL;
    char* outputName = "cypher.bmp";
    char* dOrE = NULL;
    char* tempKey = NULL;
    char key[11];

    int keyArray[10] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0};

    if(argc == 2) {
        fileName = argv[1];
    }
    else if (argc == 3) {
        fileName = argv[1];
        outputName = argv[2];
    }
    else if (argc == 4) {
        fileName = argv[1];
        outputName = argv[2];
        dOrE = argv[3];
        decrypt = (strcmp(dOrE, "d") == 0);
    }
    else if (argc == 5) {
        fileName = argv[1];
        outputName = argv[2];
        dOrE = argv[3];
        decrypt = (strcmp(dOrE, "d") == 0);
        strncpy(key, argv[4], 10);
        key[10] = '\0';
        parseKey(key, keyArray);
    }
    else {
        fileName = "penguin.bmp";
    }

    char mode[4];
    do {
        printf("Enter mode (ECB, CBC, CTR): ");
        if (fgets(mode, sizeof(mode), stdin) != NULL)
        {
            //Remove the newline character if present
            mode[strcspn(mode, "\n")] = 0;
            //Verify the mode is valid
            if (strcmp(mode, "ECB") == 0 || strcmp(mode, "CBC") == 0 || strcmp(mode, "CTR") == 0)
            {
                printf("Valid mode: %s\n", mode);
                break;
            } else
            {
                printf("Invalid mode. Please choose ECB, CBC, or CTR.\n\n");
            }
        }
        else
        {
            printf("Error reading input\n");
        }

    } while (1);

    if (decrypt) {
        printf("\nDecrypting %s\n", fileName);
    }
    else {
        printf("\nEncrypting %s\n", fileName);
    }

    FILE* picture;
    FILE* cypher;

    cypher = fopen(outputName, "wb");
    if (!(picture = fopen(fileName, "rb")))
    {
        printf("%s is missing, program will now exit\n", fileName);
        return 1;
    }


    unsigned char header[54];
    //Read the header
    fread(header, sizeof(unsigned char), 54, picture);
    fwrite(header, sizeof(unsigned char), 54, cypher);

    //Get picture stats
    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    short int bitPerPixel = *(short int*)&header[28];
    int multiplier = bitPerPixel / 8;

    //Dealing with Padding Rules
    if (width % 4 != 0)
    {
        width += (4 - (width % 4));
    }

    int size = multiplier * width * height;
    printf("SIZE: %d\n", size);
    unsigned char* pixelData = (unsigned char*)malloc(size);

    //Reading Pixel Data
    fread(pixelData, sizeof(unsigned char), size, picture);
    fclose(picture);

    //Generate keys
    key_generation(keyArray);


    //Decide which mode to implement
    if (strcmp(mode, "ECB") == 0)
    {
        for (int i=0; i<size; i++)
        {
            customByte plaintext[8];

            for (int j=0; j<8; j++)
            {
                plaintext[j] = (pixelData[i] >> (7 - j)) & 1;
                //printf("%d plaintext[%d]: %d\n", i, j, plaintext[j]);
            }
            //printf("\n");

            customByte* processedData;
            if(decrypt)
            {
                key_generation(keyArray);
                processedData = decryption(plaintext);
            }
            else
            {
                key_generation(keyArray);
                processedData = encryption(plaintext);
            }
            //printf("\n");

            customByte processedDataCopy[8];
            for (int k=0; k<8; k++)
            {
                processedDataCopy[k] = processedData[k];
            }
            //printf("Processed Data for pixel %d: ", i);
            //for (int k = 0; k < 8; k++) {
            //    printf("%d ", processedDataCopy[k]);
            //}
            //printf("\n");

            unsigned char processedByte = 0;
            for (int j=0; j<8; j++)
            {
                processedByte |= (processedData[j] << (7 - j));
            }

            if(fwrite(&processedByte, sizeof(unsigned char), 1, cypher) != 1)
            {
                printf("Error writing to file at byte %d\n", i);
            }
        }
    }
    else if (strcmp(mode, "CBC") == 0)
    {
        if(decrypt)
        {
            customByte iv[8] = {1, 1, 1, 0, 1, 1, 0, 1};
            customByte previousCiphertext[8];

            for (int i = 0; i < size; i++) {
                customByte ciphertext[8];
                unsigned char byte = pixelData[i];

                for (int j = 0; j < 8; j++) {
                    ciphertext[j] = (byte >> (7 - j)) & 1;
                }

                customByte* processedData = decryption(ciphertext);

                if (i == 0) {
                    for (int j = 0; j < 8; j++) {
                        processedData[j] ^= iv[j];
                    }
                } else {
                    for (int j = 0; j < 8; j++) {
                        processedData[j] ^= previousCiphertext[j];
                    }
                }

                unsigned char processedByte = 0;
                for (int j = 0; j < 8; j++) {
                    processedByte |= (processedData[j] << (7 - j));
                }
                for (int j=0; j<8; j++)
                {
                    previousCiphertext[j] = ciphertext[j]; // Update previous ciphertext
                }

                fwrite(&processedByte, sizeof(unsigned char), 1, cypher);
            }
        }
        else
        {
            customByte iv[8] = {1, 1, 1, 0, 1, 1, 0, 1};
            customByte previousCiphertext[8] = {0};

            for (int i = 0; i < size; i++) {
                customByte plaintext[8];
                unsigned char byte = pixelData[i];

                for (int j = 0; j < 8; j++) {
                    plaintext[j] = (byte >> (7 - j)) & 1;
                }

                if (i == 0) {
                    for (int j = 0; j < 8; j++) {
                        plaintext[j] ^= iv[j];
                    }
                } else {
                    for (int j = 0; j < 8; j++) {
                        plaintext[j] ^= previousCiphertext[j];
                    }
                }

                customByte* processedData = encryption(plaintext);

                unsigned char processedByte = 0;
                for (int j = 0; j < 8; j++) {
                    processedByte |= (processedData[j] << (7 - j));
                    previousCiphertext[j] = processedData[j]; // Update previous ciphertext
                }

                fwrite(&processedByte, sizeof(unsigned char), 1, cypher);
            }
        }
    }
    else if (strcmp(mode, "CTR") == 0)
    {
        customByte nonce[8] = {0, 0, 0, 0, 0, 0, 0, 1};
        int count = 0;

        for (int i=0; i<size; i++)
        {
            customByte plaintext[8];
            unsigned char byte = pixelData[i];

            for (int j=0; j<8; j++)
            {
                plaintext[j] = (byte >> (7-j)) & 1;
            }

            customByte counterBlock[8];
            for (int j = 0; j < 8; j++)
            {
                counterBlock[j] = nonce[j];
            }
            for (int j = 7; j >= 0; j--)
            {
                counterBlock[j] |= ((count >> (7 - (7 - j))) & 1);
            }
            customByte* counterEncrypted = encryption(counterBlock);

            customByte* processedData = malloc(8 * sizeof(customByte));
            for (int j = 0; j < 8; j++)
            {
                processedData[j] = plaintext[j] ^ counterEncrypted[j];
            }

            unsigned char processedByte = 0;
            for(int j=0; j<8; j++)
            {
                processedByte |= (processedData[j] << (7 - j));
            }

            fwrite(&processedByte, sizeof(unsigned char), 1, cypher);

            count++;
        }
    }
    

    fclose(cypher);
    free(pixelData);

    printf("Operation completed successfully. \n");
    return 0;
}
