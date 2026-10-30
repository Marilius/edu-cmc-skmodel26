# mult.py

import os

import matplotlib.pyplot as plt
import numpy as np

out_dir = "frames"
os.makedirs(out_dir, exist_ok=True)


def load_grid(path):
    with open(path) as f:
        nx = int(f.readline())
        ny = int(f.readline())
        nz = int(f.readline())

        data = np.loadtxt(f)

    grid = data.reshape((nx, ny, nz))
    return grid

global_min = float("inf")
global_max = float("-inf")

N = 50
for n_grid in range(N):
    grid = load_grid(
        f"results/run_logs_L1/grids/{n_grid}_grid.txt"
    )

    global_min = min(global_min, grid.min())
    global_max = max(global_max, grid.max())


dt = 1 / N

step = 4
threshold = 0.1
L = 1

for n_grid in range(N):
    print(f'{n_grid}_grid')
    grid = load_grid(f"results/run_logs_L1/grids/{n_grid}_grid.txt")
    nx, ny, nz = grid.shape

    hx = L / nx
    hy = L / ny
    hz = L / nz

    x, y, z = np.indices((nx, ny, nz))

    x = x * hx
    y = y * hy
    z = z * hz

    x = x[::step, ::step, ::step]
    y = y[::step, ::step, ::step]
    z = z[::step, ::step, ::step]

    vals = grid[::step, ::step, ::step]
    mask = np.abs(vals) > threshold

    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection="3d")

    scatter = ax.scatter(
        x[mask],
        y[mask],
        z[mask],
        c=vals[mask],
        cmap="viridis",
        s=7,
        vmin=global_min,
        vmax=global_max,
    )

    # Медленное вращение камеры
    elev = 25
    azim = 45 + 60 * n_grid / N

    ax.view_init(
        elev=elev,
        azim=azim,
    )

    ax.set_xlim(0, L)
    ax.set_ylim(0, L)
    ax.set_zlim(0, L)

    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")

    ax.set_title(f"Время {n_grid*dt:.2}/1 сек")

    plt.colorbar(scatter, ax=ax, label="u")

    fig.tight_layout()

    # plt.show()
    plt.savefig(
        f"{out_dir}/frame_{n_grid:03d}.png",
        dpi=200,
        # bbox_inches="tight",
    )

    plt.close(fig)
