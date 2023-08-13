#include <potentials_method.h>
#include <algorithm>
#include <limits>
#include <cmath>
#include <stdexcept>

using namespace std;

PotentialsMethod::PotentialsMethod(GreedyTable table) : table(table), u(table.k()), v(table.n()), differences(table.k(), table.n()) {}

void PotentialsMethod::calc_differences()
{
    for (size_t i = 0; i < table.k(); ++i)
	{
		for (size_t j = 0; j < table.n(); ++j)
		{
			differences[i][j] = table[i][j] - (u[i] + v[j]);
		}
	}
}

void PotentialsMethod::calc_potentials()
{
    fill(u.begin(), u.end(), numeric_limits<double>::quiet_NaN());
    fill(v.begin(), v.end(), numeric_limits<double>::quiet_NaN());

	u[0] = 0.0;

	while (true)
    {
        bool nan_u = false;
        for (auto val : u)
        {
            if (std::isnan(val))
            {
                nan_u = true;
                break;
            }
        }

        bool nan_v = false;
        for (auto val : v)
        {
            if (std::isnan(val))
            {
                nan_v = true;
                break;
            }
        }

		if (!nan_u && !nan_v)
		{
			break;
		}

		for (size_t i = 0; i < table.k(); ++i)
		{
			for (size_t j = 0; j < table.n(); ++j)
			{
                if (isnan(table.plan[i][j]))
				{
					continue;
				}

                if (isnan(u[i]) && isnan(v[j]))
				{
					continue;
				}

                if (isnan(u[i]))
				{
					u[i] = table[i][j] - v[j];
				}
                else if (isnan(v[j]))
				{
					v[j] = table[i][j] - u[i];
				}
			}
		}
	}
}

bool PotentialsMethod::is_optimal()
{
	calc_potentials();
	calc_differences();

	for (size_t i = 0; i < table.k(); ++i)
    {
        for (size_t j = 0; j < table.n(); ++j)
		{
            if (differences[i][j] < -numeric_limits<double>::epsilon())
			{
				return false;
			}
		}
	}
	return true;
}

void PotentialsMethod::optimize()
{
    top = { 0, 0 };
	double abs_min = differences[0][0];
	for (size_t i = 0; i < table.k(); ++i)
	{
		for (size_t j = 0; j < table.n(); ++j)
		{
			if (differences[i][j] < abs_min)
			{
				abs_min = differences[i][j];
				top = { i, j };
			}
		}
	}

	if (!find_loop() || loop.size() < 4)
    {
        throw runtime_error("loop not found!");
	}

	int sign = 1;
	int mi = -1, mj = -1;
    double mval = numeric_limits<double>::max();

	for (const auto& it : loop)
	{
		if (sign == -1)
		{
			if (mi == -1 || mval > table.plan[it.first][it.second])
			{
				mi = it.first;
				mj = it.second;
				mval = table.plan[mi][mj];
			}
		}
		sign *= -1;
	}

	sign = 1;
	for (const auto& it : loop)
	{
		auto i = it.first;
		auto j = it.second;

        if (isnan(table.plan[i][j]))
		{
			table.plan[i][j] = 0.0;
		}
		table.plan[i][j] += mval * sign;
		sign *= -1;
	}

    table.plan[mi][mj] = numeric_limits<double>::quiet_NaN();
}

bool PotentialsMethod::find_loop()
{
    loop.clear();
	return find_loop_row(top);
}

bool PotentialsMethod::find_loop_row(Point& pos)
{
    for (Point it{ pos.first, 0 }; it.second < table.n(); ++it.second)
    {
        if (it.second == pos.second)
		{
			continue;
		}
        if (isnan(table.plan[it.first][it.second]))
		{
			continue;
		}
		if (find_loop_column(it))
		{
			loop.push_back(it);
			return true;
		}
	}
	return false;
}

bool PotentialsMethod::find_loop_column(Point& pos)
{
    for(Point it{ 0, pos.second }; it.first < table.k(); ++it.first)
    {
        if (it == top)
		{
			loop.push_back(it);
			return true;
		}
		if (it.first == pos.first)
		{
			continue;
		}
        if (isnan(table.plan[it.first][it.second]))
		{
			continue;
		}
		if (find_loop_row(it))
		{
			loop.push_back(it);
			return true;
		}
	}
	return false;
}
