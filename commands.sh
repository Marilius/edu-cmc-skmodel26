# загрузить исходники
scp -r src polus:~/
scp run_experiments.sh polus:~/

# подключение
ssh polus

# компиляция
g++ src/serial/main.cpp -std=c++11 -O2 -o solver_serial.out
g++ src/openmp/main.cpp -std=c++11 -O2 -fopenmp -o solver_openmp.out

# g++ -O2 -fopenmp -std=c++11 src/main.cpp -o solver.out

# запуск
# mpisubmit.pl -t 8 solver.out --stdout std.out --stderr std.err -- 128 20 1. 1. 1. 0.01
