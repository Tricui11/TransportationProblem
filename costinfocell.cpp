class СostInfoCell
{
public:
    int row, column;
    double cost;

    СostInfoCell(int r, int c, double cst) : row(r), column(c), cost(cst) {}

    static bool compareCells(const СostInfoCell& a, const СostInfoCell& b) { return a.cost < b.cost; }
};
