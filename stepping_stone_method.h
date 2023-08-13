#ifndef STEPPING_STONE_METHOD_H
#define STEPPING_STONE_METHOD_H

#include <vector>
#include <iostream>
#include <shipment.cpp>

using namespace std;

class SteppingStoneMethod
{
public:    
    Shipment Empty = {};
    vector<double> demand, supply;
    vector<vector<double>> costs;
    vector<vector<Shipment>> map;
    vector<vector<vector<Shipment>>> history;

    SteppingStoneMethod();

    void northWestCornerRule();

    void minimumPriceRule();

    vector<Shipment> mapToVector();

    vector<Shipment> getNeighbors(const Shipment& s, const vector<Shipment>& lst);

    vector<Shipment> getClosedPath(const Shipment& s);

    void fixDegenerateCase();

    void steppingStone();

    void addMapToHistory();
};

#endif // STEPPING_STONE_METHOD_H
