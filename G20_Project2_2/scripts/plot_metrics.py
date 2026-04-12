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
    metric_specs = [
        ("avg_waiting", "Average Waiting Time", "Ticks"),
        ("avg_turnaround", "Average Turnaround Time", "Ticks"),
        ("avg_response", "Average Response Time", "Ticks"),
        ("throughput", "Throughput", "Processes/Tick"),
        ("cpu_utilization", "CPU Utilization", "Ratio"),
        ("deadline_misses", "Deadline Misses", "Count"),
        ("migrations", "Migrations", "Count"),
    ]
    palette = ["#0e4d92", "#2e8b57", "#c75b12", "#8b1e3f", "#6a4c93", "#008b8b", "#bc6c25"]

    fig, axes = plt.subplots(3, 3, figsize=(16, 12))
    axes = axes.flatten()

    for axis, (metric_key, title, ylabel) in zip(axes, metric_specs):
        values = [float(row[metric_key]) for row in rows]
        axis.bar(algorithms, values, color=palette[: len(algorithms)])
        axis.set_title(title)
        axis.set_ylabel(ylabel)
        axis.grid(axis="y", alpha=0.3)
        axis.set_axisbelow(True)
        for idx, value in enumerate(values):
            axis.text(idx, value, f"{value:.2f}" if value < 100 else f"{value:.0f}",
                      ha="center", va="bottom", fontsize=8)

    axes[len(metric_specs)].axis("off")
    axes[len(metric_specs) + 1].axis("off")
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
