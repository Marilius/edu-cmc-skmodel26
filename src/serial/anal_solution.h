// grid_applier.h
#pragma once

#include <cmath>

static const double pi = acos(-1.0);

class AnalyticalSolution {
    private:
        const double Lx;
        const double Ly;
        const double Lz;

        const double aT;

        // точное решение У_АНАЛ
        double u(double x, double y, double z, double t) const {
            return
                sin(2 * pi * x / Lx) *
                sin(pi * y / Ly) *
                sin(pi * z / Lz) *
                cos(aT * t + 2 * pi);
        }

    public:
        explicit AnalyticalSolution(Params params)
            : Lx(params.Lx),
            Ly(params.Ly),
            Lz(params.Lz),
            aT(
                sqrt(
                    4.0 / (Lx * Lx)
                    + 1.0 / (Ly * Ly)
                    + 1.0 / (Lz * Lz)
                ))
        {}

        double valueAt(
            const Grid& grid,
            size_t i,
            size_t j,
            size_t k,
            double t
        ) const {
            return u(
                i * grid.xStep,
                j * grid.yStep,
                k * grid.zStep,
                t
            );
        }

        void fillGrid(Grid& grid, double t) const {
            for (size_t i = 0; i < grid.xSize; ++i)
                for (size_t j = 0; j < grid.ySize; ++j)
                    for (size_t k = 0; k < grid.zSize; ++k)
                        grid.set(i, j, k, valueAt(grid, i, j, k, t));
        }
};
