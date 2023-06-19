#pragma once
#include <fstream>
#include <stdexcept>
#include <vector>

namespace NNUE {
    class Vector {
        friend class Matrix;
        friend class Layer;
        friend class Network;
        uint64_t size;
        std::vector<float> data;
        float &operator[](int i) { return data[i]; }

    public:
        Vector(int size) : size(size) { data.resize(size); }
        Vector(const std::vector<float> &data) : data(data) { size = data.size(); }
        const float &operator[](int i) const { return data[i]; }
        Vector operator+(const Vector &vec) const;
    };
    class Matrix {
        friend class Layer;
        friend class Network;
        uint64_t rows, cols;
        std::vector<Vector> data;
        Vector &operator[](int i) { return data[i]; }

    public:
        Matrix(int rows, int cols);
        Matrix(const std::vector<std::vector<float>> &data);
        Vector operator*(const Vector &vec) const;
        Matrix operator+(const Matrix &mat) const;
        const Vector &operator[](int i) const { return data[i]; }
    };
    class Layer {
        friend class Network;
        Matrix weights;
        Vector bias;
        Vector output;
        // just implement the activation from Yu Nasu's paper
        float activation(float in) {
            if (in < 0) return 0;
            if (in > 1) return 1;
            return in;
        }

    public:
        Layer(int inputSize, int outputSize) : weights(outputSize, inputSize), bias(outputSize), output(outputSize) {}
        // Layer(const Matrix &weights, const std::vector<float> &bias) : weights(weights), bias(bias) {}
        Vector &feed(const Vector &vec);
        void randomize();
        void load(std::ifstream &filename);
        void write(std::ofstream &filename) const;
    };
}