// main.cpp

#include <iostream>
#include <cassert>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cmath>
#include <algorithm>

#include "../common/params.h"
#include "grid.h"
#include "anal_solution.h"
#include "../common/error.h"
#include <iomanip>

static const std::string LOG_DIR = "./run_logs";

// коэф из ур
static const double aSquared = 1.0 / (4.0 * pi * pi);

// считаем первый временной слой u^1, возвращаем ошибку
Error initGridT1(
    const Grid& gridT0,
    Grid& gridT1,
    const AnalyticalSolution& analyticalSolution,
    double timeStep
) {
    double maxError = 0.0;
    double sumError = 0.0;
    size_t count =
        (gridT1.xSize - 2) *
        (gridT1.ySize - 2) *
        (gridT1.zSize - 2);

    for (size_t i = 1; i < gridT1.xSize - 1; ++i)
        for (size_t j = 1; j < gridT1.ySize - 1; ++j)
            for (size_t k = 1; k < gridT1.zSize - 1; ++k) {
                double prev_u = gridT0.get(i, j, k);

                // формула 12 из отчёта
                double new_u =
                    prev_u +
                    aSquared *
                    timeStep *
                    timeStep *
                    0.5 *
                    gridT0.getLaplace(i, j, k);

                gridT1.set(i, j, k, new_u);

                double curError =
                    std::fabs(
                        new_u -
                        analyticalSolution.valueAt(
                            gridT1,
                            i,
                            j,
                            k,
                            timeStep
                        )
                    );

                maxError = std::max(maxError, curError);
                sumError += curError;
            }

    Error error;
    error.maxError = maxError;
    error.avgError = sumError / static_cast<double>(count);

    return error;
}

// Логирование ошибок и сетки
// для мультика....
void logResults(
    const Grid& grid,
    const Error& error,
    size_t timeStep,
    bool logErrors,
    bool logGrids
) {
    if (logErrors) {
        std::ofstream maxErrorFile(
            LOG_DIR + "/max_errors.txt",
            std::ios::app);

        std::ofstream avgErrorFile(
            LOG_DIR + "/avg_errors.txt",
            std::ios::app);

        maxErrorFile << error.maxError << std::endl;
        avgErrorFile << error.avgError << std::endl;

        maxErrorFile.close();
        avgErrorFile.close();
    }

    if (logGrids) {
        mkdir((LOG_DIR + "/grids").c_str(), 0700);

        std::stringstream gridLogPath;
        gridLogPath
            << LOG_DIR
            << "/grids/"
            << timeStep
            << "_grid.txt";

        std::ofstream gridFile(gridLogPath.str());

        gridFile << grid.xSize << std::endl;
        gridFile << grid.ySize << std::endl;
        gridFile << grid.zSize << std::endl;

        for (size_t i = 0; i < grid.xSize; ++i)
            for (size_t j = 0; j < grid.ySize; ++j)
                for (size_t k = 0; k < grid.zSize; ++k)
                    gridFile << grid.get(i, j, k) << std::endl;

        gridFile.close();
    }
}

int main(int argc, char* argv[]) {
    timespec startTime{};
    timespec endTime{};

    Params params(argc, argv);

    std::cout << "Running task with params:\n";
    std::cout << "gridSize = " << params.gridSize << '\n';
    std::cout << "timeGridSize = " << params.timeGridSize << '\n';
    std::cout << "Lx = " << params.Lx << '\n';
    std::cout << "Ly = " << params.Ly << '\n';
    std::cout << "Lz = " << params.Lz << '\n';
    std::cout << "T = " << params.T << std::endl;

    const bool logErrors = params.logErrors;
    const bool logGrids = params.logGrids;

    if (logErrors || logGrids) {
        struct stat st = {0};

        if (stat(LOG_DIR.c_str(), &st) == -1)
            mkdir(LOG_DIR.c_str(), 0700);
    }

    const double timeStep = params.timeStep;

    clock_gettime(CLOCK_MONOTONIC, &startTime);

    // точное аналитическое решение
    AnalyticalSolution analyticalSolution(params);

    // u^0 такое
    Grid prevGrid(params);
    analyticalSolution.fillGrid(prevGrid, 0.0);
    
    logResults(
        prevGrid,
        {},
        0,
        logErrors,
        logGrids
    );
    
    // u^1 такое
    Grid curGrid(params);
    Error error =
        initGridT1(
            prevGrid,
            curGrid,
            analyticalSolution,
            timeStep
        );

    logResults(
        curGrid,
        error,
        1,
        logErrors,
        logGrids
    );

    // число узлов, где считаем
    size_t count =
        params.gridSize *
        (params.gridSize - 2) *
        (params.gridSize - 2);

    // u^{n+1}
    Grid newGrid(params);
    for (size_t t = 2; t < params.timeGridSize; ++t) {
        // TODO обнулять только границы?
        newGrid.clearGrid();

        double maxError = 0.0;
        double sumError = 0.0;

        for (size_t i = 0; i < newGrid.xSize; ++i) {
            for (size_t j = 1; j < newGrid.ySize - 1; ++j) {
                for (size_t k = 1; k < newGrid.zSize - 1; ++k) {
                    // конечно-разностная схема, мб добавить номер в отчёте
                    double uValue =
                        timeStep *
                        timeStep *
                        aSquared *
                        curGrid.getLaplace(i, j, k)
                        +
                        2.0 *
                        curGrid.get(i, j, k)
                        -
                        prevGrid.get(i, j, k);

                    newGrid.set(i, j, k, uValue);

                    double curError =
                        std::fabs(
                            uValue -
                            analyticalSolution.valueAt(
                                newGrid,
                                i,
                                j,
                                k,
                                timeStep * t
                            )
                        );

                    maxError =
                        std::max(
                            maxError,
                            curError);

                    sumError += curError;
                }
            }
        }

        error.maxError = maxError;
        error.avgError = sumError / static_cast<double>(count);

        logResults(
            newGrid,
            error,
            t,
            logErrors,
            logGrids
        );

        // prev <- cur
        // cur <- new
        prevGrid.moveValues(curGrid);
        curGrid.moveValues(newGrid);
    }

    // победаы
    clock_gettime(CLOCK_MONOTONIC, &endTime);

    double elapsed = (endTime.tv_sec - startTime.tv_sec)
        + (endTime.tv_nsec - startTime.tv_nsec) * 1e-9;

    std::cout 
        << std::fixed
        << std::setprecision(9)
        << "Task took "
        << elapsed
        << " s (~"
        << elapsed / params.timeGridSize
        << " s per iteration)"
        << std::endl;

    std::cout
        << "Avg error = "
        << error.avgError
        << std::endl;

    std::cout
        << "Max error = "
        << error.maxError
        << std::endl;

    std::cout
        << "Task finished successfully!"
        << std::endl;

    return 0;
}