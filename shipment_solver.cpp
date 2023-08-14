#include <vector>
#include <shipment.cpp>
#include <potentials_method.h>
#include <stepping_stone_method.h>
#include <cmath>

using namespace std;

class ShipmentSolver
{
public:
    Shipment Empty = {};
    vector<double> demand, supply;
    vector<vector<double>> costs;
    vector<vector<Shipment>> map;
    vector<vector<vector<Shipment>>> history;

    bool processByPotentialsMethod(bool isMinPriceRule)
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
        IsOptimalResult result = optimizer.is_optimal();
        if (result == IsOptimalResult::TimeOut)
        {
            return false;
        }
        while (result != IsOptimalResult::Optimal)
        {
            auto shipmentMap = GetShipmentMapFromPlanMatrix(optimizer.table.plan);
            history.push_back(shipmentMap);
            optimizer.optimize();
            result = optimizer.is_optimal();
            if (result == IsOptimalResult::TimeOut)
            {
                return false;
            }
        }

        map = GetShipmentMapFromPlanMatrix(optimizer.table.plan);
        history.push_back(map);

        return true;
    }

    void processBySteppingStoneMethod(bool isMinPriceRule)
    {
        SteppingStoneMethod *solver = new SteppingStoneMethod();
        solver->demand = demand;
        solver->supply = supply;
        solver->costs = costs;
        solver->map = map;
        solver->history = history;
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

    vector<vector<Shipment>> GetShipmentMapFromPlanMatrix(PlanMatrix planMatrix)
    {
        vector<vector<Shipment>> shipmentMap;
        for (size_t i = 0; i < costs.size(); ++i)
        {
            vector<Shipment> rowShipment;
            for (size_t j = 0; j < costs[i].size(); ++j)
            {
                if(!std::isnan(planMatrix[i][j]))
                {
                    double value = planMatrix[i][j];
                    auto newShipment = Shipment(value, costs[i][j], i, j);
                    rowShipment.push_back(newShipment);
                }
                else
                {
                    rowShipment.push_back(Empty);
                }
            }
            shipmentMap.push_back(rowShipment);
        }
        return shipmentMap;
    }
};
