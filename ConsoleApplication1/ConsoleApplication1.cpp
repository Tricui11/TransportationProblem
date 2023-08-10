#include <iomanip>
#include <iostream>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Shipment
{
public:
    double costPerUnit;
    int row, column;
    double quantity;

    Shipment() : quantity(0), costPerUnit(0), row(-1), column(-1) {}

    Shipment(double q, double cpu, int row, int column) : quantity(q), costPerUnit(cpu), row(row), column(column) {}

    bool operator==(const Shipment& other) const
    {
        return costPerUnit == other.costPerUnit && quantity == other.quantity && row == other.row && column == other.column;
    }

    bool operator!=(const Shipment& other) const
    {
        return !(*this == other);
    }

    static Shipment Empty;
};
Shipment Shipment::Empty = {};

vector<int> demand, supply;
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

    vector<int> source, target;
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

void northWestCornerMethod()
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
            if (supply[row] == 0)
            {
                northwest = column;
                break;
            }
        }
    }
}

vector<Shipment> mapToVector()
{
    vector<Shipment> result;
    for (auto& row : map)
    {
        for (auto& s : row)
        {
            if (s != Shipment::Empty)
            {
                result.push_back(s);
            }
        }
    }
    return result;
}

vector<Shipment> getNeighbors(const Shipment& s, const vector<Shipment>& lst)
{
    vector<Shipment> nbrs(2);
    for (auto& o : lst)
    {
        if (o != s)
        {
            if (o.row == s.row && nbrs[0] == Shipment::Empty)
            {
                nbrs[0] = o;
            }
            else if (o.column == s.column && nbrs[1] == Shipment::Empty)
            {
                nbrs[1] = o;
            }
            if (nbrs[0] != Shipment::Empty && nbrs[1] != Shipment::Empty)
            {
                break;
            }
        }
    }
    return nbrs;
}

vector<Shipment> getClosedPath(const Shipment& s)
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
                [&path](Shipment& ship)
                {
                    auto nbrs = getNeighbors(ship, path);
                    return nbrs[0] == Shipment::Empty || nbrs[1] == Shipment::Empty;
                }),
            path.end());
    } while (before != path.size());

    // place the remaining elements in the correct plus-minus order
    vector<Shipment> stones(path.size());
    fill(stones.begin(), stones.end(), Shipment::Empty);
    auto prev = s;
    for (size_t i = 0; i < stones.size(); i++)
    {
        stones[i] = prev;
        prev = getNeighbors(prev, path)[i % 2];
    }
    return stones;
}

void fixDegenerateCase()
{
    double eps = DBL_MIN;
    if (supply.size() + demand.size() - 1 != mapToVector().size())
    {
        for (size_t row = 0; row < supply.size(); row++)
        {
            for (size_t column = 0; column < demand.size(); column++)
            {
                if (map[row][column] == Shipment::Empty)
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

void steppingStone()
{
    double maxReduction = 0;
    vector<Shipment> move;
    Shipment leaving;
    bool isNull = true;

    fixDegenerateCase();

    for (size_t r = 0; r < supply.size(); r++)
    {
        for (size_t c = 0; c < demand.size(); c++)
        {
            if (map[r][c] != Shipment::Empty)
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
            map[s.row][s.column] = s.quantity == 0 ? Shipment::Empty : s;
            plus = !plus;
        }
        steppingStone();
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

void process(string filename)
{
    init(filename);
    northWestCornerMethod();
    steppingStone();
    printResult(filename);
}

int main()
{
    process("input1.txt");
    process("input2.txt");
    process("input3.txt");

    return 0;
}