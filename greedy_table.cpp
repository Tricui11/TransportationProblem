#include <greedy_table.h>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include <costinfocell.cpp>

GreedyTable::GreedyTable(const Matrix& traffic, const vector<double>& _supply, const vector<double>& _demand)
    : Matrix(traffic), supply(_supply), demand(_demand)
{
    auto supply_sum = std::accumulate(supply.begin(), supply.end(), 0.0);
    auto demand_sum = std::accumulate(demand.begin(), demand.end(), 0.0);

    if (supply_sum > demand_sum)
    {
        auto new_n = n() + 1;
        for (auto& row : *this)
        {
            row.resize(new_n);
        }
        demand.push_back(supply_sum - demand_sum);
    }
    else if (supply_sum < demand_sum)
    {
        throw std::runtime_error("Check the balance!");
    }

    plan = PlanMatrix(k(), n());
}

void GreedyTable::FillTableByMinimumPriceMethod()
{
    int total = 0;
    for (size_t i = 0; i < supply.size(); i++)
    {
        total += supply[i];
    }
    vector<СostInfoCell> sortedCells;

    for (size_t i = 0; i < (*this).size(); ++i)
    {
        for (size_t j = 0; j < (*this)[i].size(); ++j)
        {
            sortedCells.push_back(СostInfoCell(i, j, (*this)[i][j]));
        }
    }

    sort(sortedCells.begin(), sortedCells.end(), СostInfoCell::compareCells);

    do
    {
        const СostInfoCell& firstCell = sortedCells.front();
        int row = firstCell.row;
        int column = firstCell.column;
        int quantity = min(supply[row], demand[column]);
        if (quantity > 0)
        {
            plan[row][column] = quantity;
            supply[row] -= quantity;
            demand[column] -= quantity;
            total -= quantity;
        }

        sortedCells.erase(sortedCells.begin());
    } while (total != 0);
}

void GreedyTable::FillTableByNorthWestCornerMethod()
{
    double quantity;
    for (size_t i = 0; i < k(); i++)
    {
        for (size_t j = 0; j < n(); j++)
        {
            if (demand[j] == 0.0)
            {
                continue;
            }

            quantity = std::min(supply[i], demand[j]);
            plan[i][j] = quantity;
            supply[i] -= quantity;
            demand[j] -= quantity;

            if (supply[i] == 0.0)
            {
                break;
            }
        }
    }
}

double GreedyTable::CalcTotalCost() const
{
    double total = 0.0;
    for (size_t i = 0; i < k(); i++)
    {
        for (size_t j = 0; j < n(); j++)
        {
            if (!std::isnan(plan[i][j]))
            {
                total += (*this)[i][j] * plan[i][j];
            }
        }
    }
    return total;
}
