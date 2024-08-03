#!/usr/bin/env python3
import numpy as np
import matplotlib.image
import subprocess as sp

# TODO: pass all of these as CLI arguments and parse with argparse
CELL_SIZE: int = 10  # Each cell occupies this many pixels
ROWS, COLS = 32, 32
STEPS: int = 500
FPS: int = 14        # Movie FPS
it: int = 0


def check_neighbors(
    arr: np.ndarray,
    r: int,
    c: int,
):
    # Idea: instead of ignoring edges, add virtual pad to make all edges 0
    arr = np.pad(arr, (1, 1))
    r += 1
    c += 1
    cnt: int = 0
    R_DIM: int = len(arr) - 1
    C_DIM: int = len(arr[0]) - 1

    if 0 < r < R_DIM:
        if arr[r + 1][c] == 1:
            cnt += 1

        if arr[r - 1][c] == 1:
            cnt += 1

        if 0 < c < C_DIM:
            if arr[r + 1][c + 1] == 1:
                cnt += 1
            if arr[r + 1][c - 1] == 1:
                cnt += 1

            if arr[r - 1][c + 1] == 1:
                cnt += 1
            if arr[r - 1][c - 1] == 1:
                cnt += 1

            if arr[r][c + 1] == 1:
                cnt += 1
            if arr[r][c - 1] == 1:
                cnt += 1
    return cnt


def iterate(arr):
    global it
    it += 1
    for r, row in enumerate(arr):
        for c, col in enumerate(row):
            neighbors = check_neighbors(arr, r, c)
            if arr[r][c]:
                if neighbors < 2:
                    arr[r][c] = 0
                if neighbors > 3:
                    arr[r][c] = 0

            if not arr[r][c]:
                if neighbors == 3:
                    arr[r][c] = 1

    return arr


def to_image(a):
    global it
    a = np.repeat(a, CELL_SIZE, axis=0)
    a = np.repeat(a, CELL_SIZE, axis=1)
    matplotlib.image.imsave(f"./output_{it:03}.jpg", a)
    print(f"Outputting image #{it:03}", end="\r")


def make_movie():
    ffmpeg_command = f"""ffmpeg -framerate {FPS} -i './output_%03d.jpg' -c:v libx264 -pix_fmt yuv420p -vf "crop=trunc(iw/2)*2:trunc(ih/2)*2:0:0, scale=1280:720" out.mp4 && rm ./*.jpg"""

    sp.run(
        ffmpeg_command,
        shell=True,
        executable="/bin/bash",
    )


def main():
    arr: np.ndarray = np.random.randint(0, 2, (ROWS, COLS))  # initial condition

    for _ in range(STEPS):
        to_image(arr)
        arr = iterate(arr)

    make_movie()


if __name__ == "__main__":
    main()
