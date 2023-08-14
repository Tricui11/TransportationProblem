#ifndef POTENTIALS_METHOD_H
#define POTENTIALS_METHOD_H

#include <greedy_table.h>
#include <point.cpp>
#include <is_optimal_result.cpp>

class PotentialsMethod
{
public:
    GreedyTable table;

    PotentialsMethod(GreedyTable table);

    PotentialsMethod() = default;

    IsOptimalResult is_optimal();

    void optimize();

private:
    Matrix differences;
    std::vector<double> u, v;
    Point top;
    std::vector<Point> loop;

    void calc_differences();

    bool calc_potentials();

    bool isTimeOut() const;

    bool find_loop();

    bool find_loop_row(Point& pos);

    bool find_loop_column(Point& pos);
};

#endif // POTENTIALS_METHOD_H
