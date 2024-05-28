#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

void convertTo2D(uint8_t arr[16], int n, uint8_t matrix[4][4]) {
    int size = sqrt(n);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[i][j] = arr[i*size + j];
        }
    }
}

void convertTo1D(uint8_t matrix[4][4], int n, uint8_t arr[16]) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            arr[i*n + j] = matrix[i][j];
        }
    }
}


int main() {
    uint8_t arr[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

    uint8_t matrix_2[4][4];
    
    convertTo2D(arr, 16, matrix_2);
    
    // In ra mảng 2 chiều sau khi chuyển đổi
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            printf("%d ", matrix_2[i][j]);
        }
        printf("\n");
    }


    convertTo1D(matrix_2, 4, arr);

    for (int i = 0; i < 16; ++i)
    {
        printf("%d, ", arr[i]);
    }
    printf("\n");
    
    
    return 0;
}
