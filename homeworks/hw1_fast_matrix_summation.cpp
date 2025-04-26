#include <iostream>
#include <vector>
#include <chrono>
#include <random>

const int SIZE = 4096;

// Basic function to access matrix elements
int getElement(const std::vector<std::vector<int>>& matrix, int row, int col) {
    return matrix[row][col];
}

// Basic function to add two integers
int add(int a, int b) {
    return a + b;
}

// Unoptimized summation function
long long sumMatrixBasic(const std::vector<std::vector<int>>& matrix) {
    long long sum = 0;
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            sum = add(sum, getElement(matrix, i, j));
        }
    }
    return sum;
}

/* 
    Optimized function to sum the matrix
    Using direct access to the matrix elements + loop unrolling
*/
long long sumMatrixOptimized(const std::vector<std::vector<int>>& matrix) {
    long long sum = 0;
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; j+= 32) {
            sum += matrix[i][j] + matrix[i][j+1] + matrix[i][j+2] + matrix[i][j+3] +
                   matrix[i][j+4] + matrix[i][j+5] + matrix[i][j+6] + matrix[i][j+7] +
                   matrix[i][j+8] + matrix[i][j+9] + matrix[i][j+10] + matrix[i][j+11] +
                   matrix[i][j+12] + matrix[i][j+13] + matrix[i][j+14] + matrix[i][j+15] +
                   matrix[i][j+16] + matrix[i][j+17] + matrix[i][j+18] + matrix[i][j+19] +
                   matrix[i][j+20] + matrix[i][j+21] + matrix[i][j+22] + matrix[i][j+23] +
                   matrix[i][j+24] + matrix[i][j+25] + matrix[i][j+26] + matrix[i][j+27] +
                   matrix[i][j+28] + matrix[i][j+29] + matrix[i][j+30] + matrix[i][j+31];
        }
    }
    return sum;
}

/*
    Another attempt that uses pointer arithmetic to access the matrix elements.
    Produces segmentation fault because 2D vectors are not stored contiguously in memory.
*/
long long sumMatrixOptimized2(const std::vector<std::vector<int>>& matrix) {
    long long sum = 0;
    const auto *end = &matrix[SIZE][SIZE];
    for (auto *ptr = &matrix[0][0]; ptr != end; ++ptr) {
        sum += *ptr;
    }
    return sum;
}

int main() {
    // Generate a large random matrix
    std::vector<std::vector<int>> matrix(SIZE, std::vector<int>(SIZE));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(-100, 100);
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            matrix[i][j] = distrib(gen);
        }
    }
    
    /*
    This section shows that the 2D vector is not stored contiguously in memory.

    for (int i = 0; i < SIZE-1; ++i) {
        if (&matrix[i][SIZE] != &matrix[i+1][0]) {
            std::cout << "Matrix[" << i << "][SIZE]: " << &matrix[i][SIZE] << " -- Matrix[" << i+1 << "][SIZE]: " << &matrix[i+1][SIZE] << std::endl;
        }
    }
    */

    // benchmarking the basic matrix summation function
    auto start = std::chrono::high_resolution_clock::now();
    long long sum = sumMatrixBasic(matrix);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Basic Sum: " << sum << std::endl;
    std::cout << "Basic Time: " << duration.count() << " milliseconds" << std::endl;

    // benchmarking optimized version
    auto start_optimized = std::chrono::high_resolution_clock::now();
    long long optimized_sum = sumMatrixOptimized(matrix);
    auto end_optimized = std::chrono::high_resolution_clock::now();
    auto duration_optimized = std::chrono::duration_cast<std::chrono::milliseconds>(end_optimized - start_optimized);

    std::cout << "Optimized Sum: " << optimized_sum << std::endl;
    std::cout << "Optimized Time: " << duration_optimized.count() << " milliseconds" << std::endl;

    return 0;
}