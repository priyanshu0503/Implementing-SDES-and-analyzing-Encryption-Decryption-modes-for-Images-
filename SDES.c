#include "SDES.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int P10[] = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6};
int P8[] = {6, 3, 7, 4, 8, 5, 10, 9};

int key1[8], key2[8];
int IP[] = {2, 6, 3, 1, 4, 8, 5, 7};
int EP[] = {4, 1, 2, 3, 2, 3, 4, 1};
int P4[] = {2, 4, 3, 1};
int IP_inv[] = {4, 1, 3, 5, 7, 2, 8, 6};

int S0[][4] = {{1, 0, 3, 2},
               {3, 2, 1, 0},
               {0, 2, 1, 3},
               {3, 1, 3, 2}};

int S1[][4] = {{0, 1, 2, 3},
               {2, 0, 1, 3},
               {3, 0, 1, 0},
               {2, 1, 0, 3}};

void key_generation(int key[]) {
    int key_[10];
    int Ls[5], Rs[5];

    for (int i = 0; i < 10; i++) {
        key_[i] = key[P10[i] - 1];
    }

    for (int i = 0; i < 5; i++) {
        Ls[i] = key_[i];
        Rs[i] = key_[i + 5];
    }

    shift(Ls, 5, 1);
    shift(Rs, 5, 1);
    int combined[10];
    for (int i = 0; i < 5; i++) {
        combined[i] = Ls[i];
        combined[i + 5] = Rs[i];
    }

    for (int i = 0; i < 8; i++) {
        key1[i] = combined[P8[i] - 1];
    }

    shift(Ls, 5, 2);
    shift(Rs, 5, 2);
    for (int i = 0; i < 5; i++) {
        combined[i] = Ls[i];
        combined[i + 5] = Rs[i];
    }

    for (int i = 0; i < 8; i++) {
        key2[i] = combined[P8[i] - 1];
    }

    printf("KEY1 : ");
    for (int i = 0; i < 8; i++) {
        printf("%d ", key1[i]);
    }
    printf("\n");

    printf("KEY2 : ");
    for (int i = 0; i < 8; i++) {
        printf("%d ", key2[i]);
    }
    printf("\n");
}

void shift(int ar[], int n, int shifts) {
    while (shifts--) {
        int temp = ar[0];
        for (int i = 0; i < n - 1; i++) {
            ar[i] = ar[i + 1];
        }
        ar[n - 1] = temp;
    }
}

customByte* function_(customByte ar[], int key[]) {
    static customByte output[8];
    int l[4], r[4];

    for (int i = 0; i < 4; ++i) {
        l[i] = ar[i];
        r[i] = ar[i + 4];
    }

    int ep[8];
    for (int j = 0; j < 8; ++j) {
        ep[j] = r[EP[j] - 1];
    }

    for (int j = 0; j < 8; ++j) {
        ar[j] = key[j] ^ ep[j];
    }

    int l_1[4], r_1[4];
    for (int i = 0; i < 4; ++i) {
        l_1[i] = ar[i];
        r_1[i] = ar[i + 4];
    }

    int row, col, val;
    row = (l_1[0] << 1) + l_1[3];
    col = (l_1[1] << 1) + l_1[2];
    val = S0[row][col];
    customByte* str_l = convert_to_bits(val);

    row = (r_1[0] << 1) + r_1[3];
    col = (r_1[1] << 1) + r_1[2];
    val = S1[row][col];
    customByte* str_r = convert_to_bits(val);

    int r_[4];
    for (int i = 0; i < 2; ++i) {
        r_[i] = str_l[i] - '0';
        r_[i + 2] = str_r[i] - '0';
    }

    int r_p4[4];
    for (int i = 0; i < 4; ++i) {
        r_p4[i] = r_[P4[i] - 1];
    }

    for (int i = 0; i < 4; ++i) {
        l[i] = l[i] ^ r_p4[i];
    }

    for (int i = 0; i < 4; ++i) {
        output[i] = l[i];
        output[i + 4] = r[i];
    }

    return output;
}

customByte* swap(customByte* ar, int n) {
    static customByte output[8];
    for (int i = 0; i < n; ++i)
    {
        output[i] = ar[i + 4];
        output[i + 4] = ar[i];
    }
    return output; 
}

customByte* encryption(customByte plaintext[]) {
    customByte* ar = convert_to_bits(0);
    for (int i = 0; i < 8; ++i) {
        ar[i] = plaintext[IP[i] - 1];
    }

    customByte* temp = function_(ar, key1);
    ar = swap(temp, 4);
    temp = function_(ar, key2);

    for (int i = 0; i < 8; ++i) {
        ar[i] = temp[IP_inv[i] - 1];
    }

    return ar;
}

customByte* decryption(customByte ar[]) {
    customByte* temp = function_(ar, key2);
    ar = swap(temp, 4);
    temp = function_(ar, key1);

    for (int i = 0; i < 8; ++i) {
        ar[i] = temp[IP_inv[i] - 1];
    }

    return ar;
}

char* binary_(int val) {
    char* str = (char*)malloc(4 * sizeof(char));
    for (int i = 3; i >= 0; --i) {
        str[i] = (val % 2) + '0';
        val /= 2;
    }
    return str;
}

customByte* convert_to_bits(uint8_t byte) {
    customByte* bits = (customByte*)malloc(8 * sizeof(customByte));
    for (int i = 0; i < 8; ++i) {
        bits[i] = (byte >> (7 - i)) & 1;
    }
    return bits;
}

uint8_t convert_to_byte(customByte* bits) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; ++i) {
        byte |= (bits[i] << (7 - i));
    }
    return byte;
}
