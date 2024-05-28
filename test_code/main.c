#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ROTL8(x,shift) ((uint8_t) ((x) << (shift)) | ((x) >> (8 - (shift))))

uint8_t sbox[256];
uint8_t hexArray[16] = "hoenalruhoenalru";
uint8_t state_text[16];
uint8_t key[16] = "uwkeucarscobvydv";
uint8_t state_matrix[4][4];
uint8_t newKeyGround[16];
uint8_t expandedKey[176];


void initialize_aes_sbox(uint8_t sbox[256]) {
    uint8_t p = 1, q = 1;
    
    /* loop invariant: p * q == 1 in the Galois field */
    do {
        /* multiply p by 3 */
        p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);

        // printf("%d\n", p);
        /* divide q by 3 (equals multiplication by 0xf6) */
        q ^= q << 1;
        q ^= q << 2;
        q ^= q << 4;
        q ^= q & 0x80 ? 0x09 : 0;

        /* compute the affine transformation */
        uint8_t xformed = q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);

        sbox[p] = xformed ^ 0x63;
    } while (p != 1);

    /* 0 is a special case since it has no inverse */
    sbox[0] = 0x63;
}

void stringToHexArray(uint8_t *input, uint8_t *output, int inputLength) {
    for(int i = 0; i < inputLength; i++) {
        output[i] = input[i];
    }
}


// Hàm để thực hiện phép toán Rcon (lũy thừa của 2 trong trường GF(2^8))
uint8_t Rcon(uint8_t i) {
    uint8_t rcon[] = {
        0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36, 
        // Các giá trị khác cho các vòng sau (nếu cần)
    };
    return rcon[i - 1]; // Trả về giá trị Rcon tương ứng
}


void generateRandomKey(unsigned char *key, int keySize) {
    srand((unsigned int)time(NULL)); // Khởi tạo bộ sinh số ngẫu nhiên
    for(int i = 0; i < keySize; i++) {
        key[i] = (unsigned char)(rand() % 256); // Tạo một byte ngẫu nhiên
    }
}

// Hàm thực hiện phép thay thế S-box
uint8_t Sbox(uint8_t byte) {

    return sbox[byte];
}

// Hàm tạo khóa mở rộng từ khóa gốc
void KeyExpansion(uint8_t* key, uint8_t* expandedKey) {
    uint8_t temp[4];

    // Sao chép khóa gốc vào khóa mở rộng
    for (int i = 0; i < 16; i++) {
        expandedKey[i] = key[i];
    }

    // Bắt đầu tạo khóa mở rộng
    for (int i = 16; i < 176; i += 4) {
        // Lấy 4 byte cuối cùng từ khóa mở rộng trước đó
        for (int j = 0; j < 4; j++) {
            temp[j] = expandedKey[i - 4 + j];
        }

        // Mỗi 16 byte, thực hiện phép toán trên temp
        if (i % 16 == 0) {
            // Phép toán RotWord
            uint8_t t = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;

            // Phép toán SubWord (S-box)
            for (int j = 0; j < 4; j++) {
                temp[j] = sbox[temp[j]];
            }

            // Phép toán Rcon
            temp[0] = temp[0] ^ Rcon(i / 16);
        }

        // XOR temp với 4 byte trước đó của khóa mở rộng để tạo ra 4 byte mới
        for (int j = 0; j < 4; j++) {
            expandedKey[i + j] = expandedKey[i - 16 + j] ^ temp[j];
        }
    }
}


uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    uint8_t hi_bit_set;
    for (int counter = 0; counter < 8; counter++) {
        if ((b & 1) == 1) 
            p ^= a;
        hi_bit_set = (a & 0x80);
        a <<= 1;
        if (hi_bit_set == 0x80) 
            a ^= 0x1b;      
        b >>= 1;
    }
    return p;
}

// Hàm thực hiện bước MixColumns
void mixColumns(uint8_t state[4][4]) {
    uint8_t tmp[4][4];
    int i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            tmp[j][i] = gmul(state[j][i], 2) ^ 
                        gmul(state[(j + 1) % 4][i], 3) ^ 
                        gmul(state[(j + 2) % 4][i], 1) ^ 
                        gmul(state[(j + 3) % 4][i], 1);
        }
    }
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            state[j][i] = tmp[j][i];
        }
    }
}

void convertTo2D(uint8_t arr[16], int n, uint8_t matrix[4][4]) {
    int size = sqrt(n);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[i][j] = arr[j*size + i];
        }
    }
}

void convertTo1D(uint8_t matrix[4][4], int n, uint8_t arr[16]) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            arr[j*n + i] = matrix[i][j];
        }
    }
}

void addGroundKey(uint8_t arr1[], uint8_t arr2[], uint8_t result[], int length){
    for (int i = 0; i < length; ++i)
    {
        result[i] = arr1[i]^arr2[i];
    }
}

void shiftRow(uint8_t matrix[4][4]){
    uint8_t p[4];
    for (int i = 0; i < 4; ++i)
    {
        for (int k = 0; k < 4; ++k)
        {
            p[k] = matrix[i][k];
        }
        for (int j = 0; j < 4; ++j)
        {
            matrix[i][j] = p[(j+i) % 4];
        }
    }
}


int main() {


    // Chuyển đổi và lưu vào hexArray
    // Thêm padding 0x00 để đủ 16 byte
    // for(int i = 8; i < 16; i++) {
    //     hexArray[i] = 0x00;
    // }
    initialize_aes_sbox(sbox);


    // generateRandomKey(key, sizeof(key)); // Tạo khóa ngẫu nhiên

    printf("Khóa 16 byte ngẫu nhiên: ");
    for(int i = 0; i < sizeof(key); i++) {
        printf("%02x", key[i]); // In khóa dưới dạng hex
        if(i < sizeof(key) - 1) printf(":");
    }
    printf("\n");
    printf("Khóa 16 byte ngẫu nhiên: ");
    for(int i = 0; i < sizeof(key); i++) {
        printf("%c", key[i]); // In khóa dưới dạng hex
    }


    KeyExpansion(key, expandedKey);

    // In khóa mở rộng ra màn hình (chỉ mục đích minh họa)
    for (int i = 0; i < 176; i++) {
        if (i % 16 == 0) printf("\n");
        printf("%02X ", expandedKey[i]);
    }

    addGroundKey(hexArray, key, state_text, 16);

    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 16; ++j)
        {
            state_text[j] = sbox[state_text[j]];
            newKeyGround[j] = expandedKey[16*(i + 1) + j];
        }
        convertTo2D(state_text, 16, state_matrix);
        shiftRow(state_matrix);
        mixColumns(state_matrix);
        convertTo1D(state_matrix, 4, state_text);
        addGroundKey(state_text, newKeyGround, state_text, 16);
        // convertTo2D(state_text, 16, state_matrix);
    }

    for (int j = 0; j < 16; ++j)
    {
        state_text[j] = sbox[state_text[j]];
    }
    convertTo2D(state_text, 16, state_matrix);
    shiftRow(state_matrix);
    convertTo1D(state_matrix, 4, state_text);
    for (int j = 0; j < 16; ++j)
    {
        newKeyGround[j] = expandedKey[16*10 + j];
    }
    addGroundKey(state_text, newKeyGround, state_text, 16);

    printf("\n");
    printf("\n");
    printf("\n");

    for (int i = 0; i < 16; ++i)
    {
        printf("%02X ", state_text[i]);
    }

    printf("\n");

    for (int i = 0; i < 16; ++i)
    {
        printf("%c ", state_text[i]);
    }
    printf("\n");
    // getchar();
    return 0;
}


// d9 b3 72 32 df ad b0 92 20 b0 e9 ec 1c 48 45 d6