#ifndef PLANMATRIX_H
#define PLANMATRIX_H

#include <matrix.cpp>
#include <limits>

class PlanMatrix : public Matrix {
public:
    PlanMatrix() = default;

    PlanMatrix(const size_t k, const size_t n) : Matrix(k, n)
    {
        for (auto& row : *this)
        {
            fill(row.begin(), row.end(), std::numeric_limits<double>::quiet_NaN());
        }
    }
};
#endif // PLANMATRIX_H
