#!/bin/bash

PI=3.141592653589793

THREADS=(1 2 4 8)
GRIDS=(128 256 512)

TIME_STEPS=20
FINAL_TIME=0.01

rm -rf results_out results_err run_logs

mkdir -p results_out
mkdir -p results_err

########################################
# SERIAL
########################################

echo "=== SERIAL, L = 1 ==="

for N in "${GRIDS[@]}"
do
    mpisubmit.pl \
        -p 1 \
        -t 1 \
        solver_serial.out \
        --stdout results_out/serial_L1_N${N}.txt \
        --stderr results_err/serial_L1_N${N}.txt.err \
        -- \
        $N $TIME_STEPS 1 1 1 $FINAL_TIME
done

echo "=== SERIAL, L = PI ==="

for N in "${GRIDS[@]}"
do
    mpisubmit.pl \
        -p 1 \
        -t 1 \
        solver_serial.out \
        --stdout results_out/serial_LPI_N${N}.txt \
        --stderr results_err/serial_LPI_N${N}.txt.err \
        -- \
        $N $TIME_STEPS $PI $PI $PI $FINAL_TIME
done

########################################
# OPENMP
########################################

echo "=== OPENMP, L = 1 ==="

for N in "${GRIDS[@]}"
do
    for T in "${THREADS[@]}"
    do
        mpisubmit.pl \
            -p 1 \
            -t $T \
            solver_openmp.out \
            --stdout results_out/openmp_L1_N${N}_T${T}.txt \
            --stderr results_err/openmp_L1_N${N}_T${T}.txt.err \
            -- \
            $N $TIME_STEPS 1 1 1 $FINAL_TIME
    done
done

echo "=== OPENMP, L = PI ==="

for N in "${GRIDS[@]}"
do
    for T in "${THREADS[@]}"
    do
        mpisubmit.pl \
            -p 1 \
            -t $T \
            solver_openmp.out \
            --stdout results_out/openmp_LPI_N${N}_T${T}.txt \
            --stderr results_err/openmp_LPI_N${N}_T${T}.txt.err \
            -- \
            $N $TIME_STEPS $PI $PI $PI $FINAL_TIME
    done
done

echo "All jobs submitted."

########################################
# VISUALIZATION DATASET
########################################

# echo "=== VISUALIZATION DATASET ==="

# mpisubmit.pl \
#     -p 1 \
#     -t 1 \
#     solver_serial.out \
#     --stdout results_out/visualization_L1_N128_T50.txt \
#     --stderr results_err/visualization_L1_N128_T50.txt.err \
#     -- \
#     128 50 1 1 1 1 log_errors log_grids

# mpisubmit.pl \
#     -p 1 \
#     -t 1 \
#     solver_serial.out \
#     --stdout results_out/visualization_LPI_N128_T50.txt \
#     --stderr results_err/visualization_LPI_N128_T50.txt.err \
#     -- \
#     128 50 $PI $PI $PI 1 log_errors log_grids
