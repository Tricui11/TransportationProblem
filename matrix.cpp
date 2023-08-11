#ifndef MATRIX_H
#define MATRIX_H

#include<vector>

using namespace std;

class Matrix : public vector<vector<double>>
{
public:
    Matrix(initializer_list<vector<double>> list) : vector<vector<double>>(list) {}

    Matrix(const size_t k, const size_t n) : vector<vector<double>>(k)
    {
        for (auto it = begin(); it != end(); it++)
        {
            it->resize(n);
        }
    }

    Matrix() = default;

    size_t k() const { return size(); }

    size_t n() const { return size() ? front().size() : 0; }
};

#endif // MATRIX_H
