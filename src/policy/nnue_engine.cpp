#include "nnue_engine.hpp"
using namespace NNUE;

Vector Vector::operator+(const Vector &vec) const {
    if (size != vec.size) {
        throw std::runtime_error("Vector addition error: dimension mismatch");
    }
    Vector res(size);
    for (uint64_t i = 0; i < size; i++) {
        res[i] = data[i] + vec[i];
    }
    return res;
}

Matrix::Matrix(int rows, int cols) : rows(rows), cols(cols) {
    data = std::vector<Vector>(rows, Vector(cols));
}

Matrix::Matrix(const std::vector<std::vector<float>> &data) {
    rows = data.size();
    cols = data[0].size();
    this->data = std::vector<Vector>(rows, Vector(cols));
    for (uint64_t i = 0; i < rows; i++) {
        this->data[i].data = data[i];
    }
}

Vector Matrix::operator*(const Vector &vec) const {
    if (cols != vec.size) {
        throw std::runtime_error("Matrix multiplication error: dimension mismatch");
    }
    Vector res(rows);
    for (uint64_t i = 0; i < rows; i++) {
        res[i] = 0;
        for (uint64_t j = 0; j < cols; j++) {
            res[i] += data[i][j] * vec[j];
        }
    }
    return res;
}

Matrix Matrix::operator+(const Matrix &mat) const {
    if (rows != mat.rows || cols != mat.cols) {
        throw std::runtime_error("Matrix addition error: dimension mismatch");
    }
    Matrix res(rows, cols);
    for (uint64_t i = 0; i < rows; i++) {
        res.data[i] = data[i];
        for (uint64_t j = 0; j < cols; j++) {
            res.data[i][j] += mat.data[i][j];
        }
    }
    return res;
}

Vector& Layer::feed(const Vector &vec) {
    auto res = (weights * vec) + bias;
    for (uint64_t i = 0; i < res.size; i++) {
        output[i] = activation(res[i]);
    }
    return output;
}

void Layer::randomize() {
    for (uint64_t i = 0; i < weights.rows; i++) {
        for (uint64_t j = 0; j < weights.cols; j++) {
            weights[i][j] = (float)rand() / RAND_MAX * 2.0 - 1.0;
        }
    }
    for (uint64_t i = 0; i < bias.size; i++) {
        bias[i] = (float)rand() / RAND_MAX * 2.0 - 1.0;
    }
}

void Layer::load(std::ifstream &fin) {
    for (uint64_t i = 0; i < weights.rows; i++) {
        for (uint64_t j = 0; j < weights.cols; j++) {
            fin.read((char *)&weights[i][j], sizeof(float));
        }
    }
    for (uint64_t i = 0; i < bias.size; i++) {
        fin.read((char *)&bias[i], sizeof(float));
    }
}

void Layer::write(std::ofstream &fout) const {
    for (uint64_t i = 0; i < weights.rows; i++) {
        for (uint64_t j = 0; j < weights.cols; j++) {
            fout.write((char *)&weights[i][j], sizeof(float));
        }
    }
    for (uint64_t i = 0; i < bias.size; i++) {
        fout.write((char *)&bias[i], sizeof(float));
    }
}