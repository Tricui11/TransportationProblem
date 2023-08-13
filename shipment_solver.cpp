#include <vector>
#include <shipment.cpp>
#include <potentials_method.h>
#include <stepping_stone_method.h>

using namespace std;

class ShipmentSolver
{
public:
    vector<double> demand, supply;
    vector<vector<double>> costs;
    vector<vector<Shipment>> map;
    vector<vector<vector<Shipment>>> history;

    void processByPotentialsMethod(bool isMinPriceRule)
    {
        Matrix matrixCosts(costs.size(), costs[0].size());
        for (size_t i = 0; i < costs.size(); ++i)
        {
            for (size_t j = 0; j < costs[i].size(); ++j)
            {
                matrixCosts[i][j] = costs[i][j];
            }
        }
        auto solution = GreedyTable(matrixCosts, supply, demand);
        if (isMinPriceRule)
        {
            solution.FillTableByMinimumPriceRule();
        }
        else
        {
            solution.FillTableByNorthWestCornerRule();
        }
        auto optimizer = PotentialsMethod(solution);
        while (!optimizer.is_optimal())
        {
            optimizer.optimize();
        }
        for (size_t i = 0; i < costs.size(); ++i)
        {
            for (size_t j = 0; j < costs[i].size(); ++j)
            {
                map[i][j] = Shipment(optimizer.table.plan[i][j], costs[i][j], i, j);
            }
        }
    }

    void processBySteppingStoneMethod(bool isMinPriceRule)
    {
        SteppingStoneMethod *solver = new SteppingStoneMethod();
        solver->demand = demand;
        solver->supply = supply;
        solver->costs = costs;
        solver->map = map;
        if (isMinPriceRule)
        {
            solver->minimumPriceRule();
        }
        else
        {
            solver->northWestCornerRule();
        }
        solver->steppingStone();
        demand = solver->demand;
        supply = solver->supply;
        costs = solver->costs;
        map = solver->map;
        history = solver->history;
        delete solver;
    }
};
