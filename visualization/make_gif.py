import imageio.v2 as imageio

frames = []
N = 50

for i in range(N):
    frames.append(
        imageio.imread(
            f"frames/frame_{i:03d}.png"
        )
    )

imageio.mimsave(
    "multik.gif",
    frames,
    fps=10,
)