#!/usr/bin/env python3

import csv
import sys
from pathlib import Path


def load_rows(path: Path):
    with path.open(newline="") as handle:
        return list(csv.DictReader(handle))


def plot_with_matplotlib(rows, output_path: Path):
    import matplotlib.pyplot as plt

    algorithms = [row["algorithm"] for row in rows]
    waiting = [float(row["avg_waiting"]) for row in rows]
    turnaround = [float(row["avg_turnaround"]) for row in rows]
    response = [float(row["avg_response"]) for row in rows]

    fig, axes = plt.subplots(1, 3, figsize=(14, 4))
    for axis, values, title in zip(
        axes,
        [waiting, turnaround, response],
        ["Average Waiting Time", "Average Turnaround Time", "Average Response Time"],
    ):
        axis.bar(algorithms, values, color=["#1f77b4", "#2ca02c", "#ff7f0e", "#d62728"][: len(algorithms)])
        axis.set_title(title)
        axis.set_ylabel("Ticks")
        axis.grid(axis="y", alpha=0.3)

    fig.tight_layout()
    fig.savefig(output_path)
    print(f"saved {output_path}")


def main():
    if len(sys.argv) != 2:
        print("usage: plot_metrics.py <comparison.csv>", file=sys.stderr)
        return 1

    csv_path = Path(sys.argv[1])
    rows = load_rows(csv_path)
    if not rows:
      print("comparison file has no rows", file=sys.stderr)
      return 1

    output_path = csv_path.with_suffix(".png")
    try:
        plot_with_matplotlib(rows, output_path)
    except ModuleNotFoundError:
        print(
            "matplotlib is not installed. Run: python3 -m pip install matplotlib",
            file=sys.stderr,
        )
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
