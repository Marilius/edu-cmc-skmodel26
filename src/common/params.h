// params.h

#pragma once

struct Params {
    size_t gridSize;
    size_t timeGridSize;
    double Lx;
    double Ly;
    double Lz;
    double T;
    bool logGrids = false;
    bool logErrors = false;

    double timeStep;

    Params(int argc, char *argv[]) {
        assert(argc >= 7);

        gridSize = std::stoi(argv[1]);
        timeGridSize = std::stoi(argv[2]);
        Lx = std::stod(argv[3]);
        Ly = std::stod(argv[4]);
        Lz = std::stod(argv[5]);
        T = std::stod(argv[6]);

        for (int i = 7; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == LOG_ERRORS)
                logErrors = true;

            if (arg == LOG_GRIDS)
                logGrids = true;
        }

        timeStep = T / (double) timeGridSize;
    }

    private:
        const std::string LOG_GRIDS = "log_grids";
        const std::string LOG_ERRORS = "log_errors";
};