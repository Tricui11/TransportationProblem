#include <greedy_table.h>
#include <point.cpp>

class PotentialsMethod
{
public:
    GreedyTable table;

    PotentialsMethod(GreedyTable table);

    PotentialsMethod() = default;

    bool is_optimal();

    void optimize();

private:
    Matrix differences;
    std::vector<double> u;
    std::vector<double> v;
    Point top;
    std::vector<Point> loop;

    void calc_differences();

    void calc_potentials();

    bool find_loop();

    bool find_loop_row(Point& pos);

    bool find_loop_column(Point& pos);
};
