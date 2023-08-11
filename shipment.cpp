#ifndef SHIPMENT_H
#define SHIPMENT_H

class Shipment
{
public:
    double costPerUnit;
    int row, column;
    double quantity;

    Shipment() : costPerUnit(0), row(-1), column(-1), quantity(0) {}

    Shipment(double q, double cpu, int row, int column) : costPerUnit(cpu), row(row), column(column), quantity(q) {}

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

#endif // SHIPMENT_H
