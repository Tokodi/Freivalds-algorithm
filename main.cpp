#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <random>
#include <thread>
#include <vector>

using Matrix = std::vector<std::vector<int>>;
using Vector = std::vector<int>;
using ThreadPool = std::vector<std::future<bool>>;

static int dimension;

bool CheckVectorEquality(const Vector& beta, const Vector& gamma) {
    if (beta.size() != gamma.size()) {
        return false;
    }

    for (int i = 0; i < dimension; ++i) {
        if (beta[i] != gamma[i]) {
            return false;
        }
    }

    return true;
}

Vector Multiply(const Matrix& A, const Vector& V) {
    Vector resultPtr(dimension);
    for (int i = 0; i < dimension; ++i) {
        for (int j = 0; j < dimension; ++j) {
            resultPtr[i] += A[i][j] * V[j];
        }
    }

    return resultPtr;
}

int GetRandomNumber(int min, int max) {
   static thread_local std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
   std::uniform_int_distribution<int> distribution(min, max);
   return distribution(generator);
}

Vector GenerateRandomBinaryVector() {
    Vector binaryVector(dimension);
    for (auto& x : binaryVector) {
        x = GetRandomNumber(0, 1);
    }

    return binaryVector;
}

bool CalculateFreivalds(const Matrix& A, const Matrix& B, const Matrix& C) {
    const Vector alpha = GenerateRandomBinaryVector();
    const Vector tmp = Multiply(B, alpha);
    const Vector beta = Multiply(A, tmp);
    const Vector gamma = Multiply(C, alpha);

    return CheckVectorEquality(beta, gamma);
}

Matrix ReadMatrixFromFile(std::ifstream& inputFile) {
    Matrix matrix(dimension, Vector(dimension));
    for (auto& vec : matrix) {
        for (auto& x : vec) {
            inputFile >> x;
        }
    }

    return matrix;
}

int main() {
    static const std::string inputFileName = "input.txt";
    static const std::string outputFileName = "output.txt";

    std::ifstream inputFile(inputFileName);

    size_t tryCount;

    inputFile >> dimension;
    inputFile >> tryCount;

    const Matrix A = ReadMatrixFromFile(inputFile);
    const Matrix B = ReadMatrixFromFile(inputFile);
    const Matrix C = ReadMatrixFromFile(inputFile);

    bool isSolution = true;

    ThreadPool threads(tryCount);
    for (auto& t : threads) {
        t = std::async(std::launch::async, CalculateFreivalds, A, B, C);
    }

    for (auto& t: threads) {
        isSolution &= t.get();
    }

    if (isSolution) {
        std::cout << "A * B == C" << std::endl;
    } else {
        std::cout << "A * B != C" << std::endl;
    }

    return 0;
}
