#include <iomanip>
#include <iostream>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>
#include <algorithm>
#include <cfloat>
#include <costinfocell.cpp>
#include <shipment.cpp>
#include <potentials_method.cpp>
#include <stepping_stone_method.h>

using namespace std;

class ShipmentSolver
{
public:


    void solve()
    {
        cout << fixed << setprecision(3);

        Matrix matrixCosts(costs.size(), costs[0].size());

        for (size_t i = 0; i < costs.size(); ++i) {
            for (size_t j = 0; j < costs[i].size(); ++j) {
                matrixCosts[i][j] = costs[i][j];
            }
        }

        cout << "Northwest corner method test" << endl;
        auto solution = GreedyTable(matrixCosts, supply, demand);
        solution.FillTableByNorthWestCornerRule();
   //     solution.FillTableByMinimumPriceRule();
        cout << "Cost of the plan = " << solution.CalcTotalCost() << endl;
        cout << endl << endl;

        cout << "Potentials method test" << endl;
        auto optimizer = PotentialsMethod(solution);
        while (!optimizer.is_optimal())
        {
            optimizer.optimize();
        }
        cout << "Cost of the plan = " << optimizer.table.CalcTotalCost() << endl;
    }


    void process()
    {
        SteppingStoneMethod *solver = new SteppingStoneMethod();
        solver->demand = demand;
        solver->supply = supply;
        solver->costs = costs;
        solver->map = map;
//        solver->northWestCornerRule();
           solver->minimumPriceRule();


 //      solve();

        solver->steppingStone();
//  //      printMap();
        demand = solver->demand;
        supply = solver->supply;
        costs = solver->costs;
        map = solver->map;
    }

//private:
    vector<double> demand, supply;
    vector<vector<double>> costs;
    vector<vector<Shipment>> map;
};
