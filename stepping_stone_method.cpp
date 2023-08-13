#include <stepping_stone_method.h>
#include <costinfocell.cpp>
#include <cfloat>
#include <iomanip>
#include <algorithm>
#include <limits>

SteppingStoneMethod::SteppingStoneMethod() {}
SteppingStoneMethod::SteppingStoneMethod() {}

Shipment Shipment::Empty = {};

void SteppingStoneMethod::northWestCornerRule()
    int northwest = 0;
    for (size_t row = 0; row < supply.size(); row++)
{
    int northwest = 0;
    for (size_t row = 0; row < supply.size(); row++)
    {
        for (size_t column = northwest; column < demand.size(); column++)
        {
            int quantity = min(supply[row], demand[column]);
            map[row][column] = Shipment(quantity, costs[row][column], row, column);
            supply[row] -= quantity;
            demand[column] -= quantity;
        for (size_t column = northwest; column < demand.size(); column++)
        {
            int quantity = min(supply[row], demand[column]);
            map[row][column] = Shipment(quantity, costs[row][column], row, column);
            supply[row] -= quantity;
            demand[column] -= quantity;
            if (supply[row] == 0)
            {
                northwest = column;
                break;
            }
        }
        }
    }
}
}

void SteppingStoneMethod::minimumPriceRule()
    int total = 0;
    for (size_t i = 0; i < supply.size(); i++)
{
    int total = 0;
    for (size_t i = 0; i < supply.size(); i++)
    {
        total += supply[i];
    }
    vector<СostInfoCell> sortedCells;

    for (size_t i = 0; i < costs.size(); ++i)
    for (size_t i = 0; i < costs.size(); ++i)
    {
        for (size_t j = 0; j < costs[i].size(); ++j)
        for (size_t j = 0; j < costs[i].size(); ++j)
        {
            sortedCells.push_back(СostInfoCell(i, j, costs[i][j]));
            sortedCells.push_back(СostInfoCell(i, j, costs[i][j]));
        }
    }

    sort(sortedCells.begin(), sortedCells.end(), СostInfoCell::compareCells);
    sort(sortedCells.begin(), sortedCells.end(), СostInfoCell::compareCells);

    do
    {
    do
    {
        const СostInfoCell& firstCell = sortedCells.front();
        int row = firstCell.row;
        int column = firstCell.column;
        int quantity = min(supply[row], demand[column]);
        if (quantity > 0)
        {
            map[row][column] = Shipment(quantity, firstCell.cost, row, column);
            supply[row] -= quantity;
            demand[column] -= quantity;
            total -= quantity;
        }
            total -= quantity;
        }
        sortedCells.erase(sortedCells.begin());
    } while (total != 0);
}

vector<Shipment> SteppingStoneMethod::mapToVector()
{
    vector<Shipment> result;
    for (auto& row : map)
    {
        for (auto& s : row)
        {
            if (s != Empty)
            {
                result.push_back(s);
            }
        }
    }
    return result;
}

vector<Shipment> SteppingStoneMethod::getNeighbors(const Shipment& s, const vector<Shipment>& lst)
{
    vector<Shipment> nbrs(2);
    for (auto& o : lst)
    {
        if (o != s)
        {
            if (o.row == s.row && nbrs[0] == Empty)
            {
                nbrs[0] = o;
            }
            else if (o.column == s.column && nbrs[1] == Empty)
            {
                nbrs[1] = o;
            }
            if (nbrs[0] != Empty && nbrs[1] != Empty)
            {
                break;
            }
        }
    }
    return nbrs;
}

vector<Shipment> SteppingStoneMethod::getClosedPath(const Shipment& s)
{
    auto path = mapToVector();
    path.insert(path.begin(), s);

    // remove (and keep removing) elements that do not have a
    // vertical AND horizontal neighbor
    size_t before;
    do
    {
        before = path.size();
        path.erase(
                    remove_if(
                        path.begin(), path.end(),
                        [&path, this](Shipment& ship)
        {
            auto nbrs = getNeighbors(ship, path);
            return nbrs[0] == Empty || nbrs[1] == Empty;
        }),
                    path.end());
    } while (before != path.size());

    // place the remaining elements in the correct plus-minus order
    vector<Shipment> stones(path.size());
    fill(stones.begin(), stones.end(), Empty);
    auto prev = s;
    for (size_t i = 0; i < stones.size(); i++)
    {
        stones[i] = prev;
        prev = getNeighbors(prev, path)[i % 2];
    }
    return stones;
}

void SteppingStoneMethod::fixDegenerateCase()
{
    double eps = DBL_MIN;
    if (supply.size() + demand.size() - 1 != mapToVector().size())
    {
        for (size_t row = 0; row < supply.size(); row++)
        {
            for (size_t column = 0; column < demand.size(); column++)
            {
                if (map[row][column] == Empty)
                {
                    Shipment dummy(eps, costs[row][column], row, column);
                    if (getClosedPath(dummy).empty())
                    {
                        map[row][column] = dummy;
                        return;
                    }
                }
            }
        }
    }
}

void SteppingStoneMethod::steppingStone()
{
    printMap();

    double maxReduction = 0;
    vector<Shipment> move;
    Shipment leaving;
    bool isNull = true;

    fixDegenerateCase();

    for (size_t r = 0; r < supply.size(); r++)
    {
        for (size_t c = 0; c < demand.size(); c++)
        {
            if (map[r][c] != Empty)
            {
                continue;
            }

            Shipment trial(0, costs[r][c], r, c);
            vector<Shipment> path = getClosedPath(trial);

            double reduction = 0;
            double lowestQuantity = INT32_MAX;
            Shipment leavingCandidate;

            bool plus = true;
            for (auto& s : path)
            {
                if (plus)
                {
                    reduction += s.costPerUnit;
                }
                else
                {
                    reduction -= s.costPerUnit;
                    if (s.quantity < lowestQuantity)
                    {
                        leavingCandidate = s;
                        lowestQuantity = s.quantity;
                    }
                }
                plus = !plus;
            }
            if (reduction < maxReduction)
            {
                isNull = false;
                move = path;
                leaving = leavingCandidate;
                maxReduction = reduction;
            }
        }
    }

    if (!isNull)
    {
        double q = leaving.quantity;
        bool plus = true;
        for (auto& s : move)
        {
            s.quantity += plus ? q : -q;
            map[s.row][s.column] = s.quantity == 0 ? Empty : s;
            plus = !plus;
        }
        steppingStone();
    }
}

void SteppingStoneMethod::printMap()
{
    double totalCosts = 0.0;
    Shipment Empty = {};
    for (size_t row = 0; row < supply.size(); row++)
    {
        for (size_t column = 0; column < demand.size(); column++)
        {
            auto s = map[row][column];
            if (s != Empty && s.row == row && s.column == column)
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
