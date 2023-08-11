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
        solution.FillTableByNorthWestCornerMethod();
   //     solution.FillTableByMinimumPriceMethod();
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


    void process(string filename)
    {
        init(filename);
        SteppingStoneMethod *solver = new SteppingStoneMethod();
        solver->demand = demand;
        solver->supply = supply;
        solver->costs = costs;
        solver->map = map;
//        solver->northWestCornerMethod();
           solver->minimumPriceMethod();


 //      solve();

        solver->steppingStone();
//  //      printMap();
        demand = solver->demand;
        supply = solver->supply;
        costs = solver->costs;
        map = solver->map;
        printResult(filename);
    }

private:
    vector<double> demand, supply;
    vector<vector<double>> costs;
    vector<vector<Shipment>> map;


    void init(string filename)
    {
        ifstream ifs;

        ifs.open(filename);
        if (!ifs)
        {
            cerr << "File not found: " << filename << '\n';
            return;
        }

        size_t numSources, numTargets;
        ifs >> numSources >> numTargets;

        vector<double> source, target;
        int t;

        for (size_t i = 0; i < numSources; i++)
        {
            ifs >> t;
            source.push_back(t);
        }

        for (size_t i = 0; i < numTargets; i++)
        {
            ifs >> t;
            target.push_back(t);
        }

        // fix imbalance
        int totalSource = accumulate(source.cbegin(), source.cend(), 0);
        int totalDestination = accumulate(target.cbegin(), target.cend(), 0);
        if (totalSource > totalDestination)
        {
            target.push_back(totalSource - totalDestination);
        }
        else if (totalDestination > totalSource)
        {
            source.push_back(totalDestination - totalSource);
        }

        supply = source;
        demand = target;

        costs.clear();
        map.clear();

        double d;
        for (size_t i = 0; i < numSources; i++)
        {
            size_t cap = max(numTargets, demand.size());

            vector<double> dt(cap);
            vector<Shipment> st(cap);
            for (size_t j = 0; j < numTargets; j++)
            {
                ifs >> d;
                dt[j] = d;
            }
            costs.push_back(dt);
            map.push_back(st);
        }
        for (size_t i = numSources; i < supply.size(); i++)
        {
            size_t cap = max(numTargets, demand.size());

            vector<Shipment> st(cap);
            map.push_back(st);

            vector<double> dt(cap);
            costs.push_back(dt);
        }


    }

    void printResult(string filename)
    {
        ifstream ifs;
        string buffer;

        ifs.open(filename);
        if (!ifs)
        {
            cerr << "File not found: " << filename << '\n';
            return;
        }

        cout << filename << "\n\n";
        while (!ifs.eof())
        {
            getline(ifs, buffer);
            cout << buffer << '\n';
        }
        cout << '\n';

        cout << "Optimal solution " << filename << "\n\n";
        double totalCosts = 0.0;
        for (size_t row = 0; row < supply.size(); row++)
        {
            for (size_t column = 0; column < demand.size(); column++)
            {
                auto s = map[row][column];
                if (s != Shipment::Empty && s.row == row && s.column == column)
                {
                    cout << ' ' << setw(3) << s.quantity << ' ';
                    totalCosts += s.quantity * s.costPerUnit;
                }
                else
                {
                    cout << "  -  ";
                }
            }
            cout << '\n';
        }
        cout << "\nTotal costs: " << totalCosts << "\n\n";
    }
};
