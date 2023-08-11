#include <matrix.cpp>
#include <plan_matrix.cpp>

class GreedyTable : public Matrix
{
public:
    vector<double> supply;
    vector<double> demand;
    PlanMatrix plan;

    GreedyTable(const Matrix&, const vector<double>&, const vector<double>&);

    GreedyTable() = default;

    void FillTableByNorthWestCornerMethod();

    void FillTableByMinimumPriceMethod();

    double CalcTotalCost() const;
};
