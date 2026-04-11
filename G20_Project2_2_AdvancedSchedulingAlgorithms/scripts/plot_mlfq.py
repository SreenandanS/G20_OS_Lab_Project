#!/usr/bin/env python3
"""
plot_mlfq.py  —  Rich visualisation for MLFQ simulation output
Author: Sukrat Singh Kushwaha

Reads the CSV files produced by the scheduler_sim:
  <out_dir>/mlfq/metrics.csv   – per-run metrics
  <out_dir>/mlfq/timeline.csv  – per-tick CPU assignments
  <out_dir>/comparison.csv     – side-by-side algorithm comparison

Produces four charts saved as PNG files:
  1. mlfq_gantt.png        – colour-coded Gantt chart (one row per CPU)
  2. mlfq_metrics_bar.png  – bar chart: waiting / turnaround / response
  3. mlfq_comparison.png   – grouped bar chart across all algorithms
  4. mlfq_cpu_util.png     – CPU utilisation pie chart

Usage:
    python3 scripts/plot_mlfq.py <output_dir>

Example:
    python3 scripts/plot_mlfq.py out/mlfq_run
"""

import csv
import sys
from pathlib import Path

# ── colour palette ──────────────────────────────────────────────────────────
# One colour per process PID (wraps for > 12 processes)
COLOURS = [
    "#4e79a7", "#f28e2b", "#e15759", "#76b7b2",
    "#59a14f", "#edc948", "#b07aa1", "#ff9da7",
    "#9c755f", "#bab0ac", "#d37295", "#86bcb6",
]

def colour(pid: int) -> str:
    return COLOURS[(pid - 1) % len(COLOURS)]


# ── CSV loaders ─────────────────────────────────────────────────────────────
def load_csv(path: Path) -> list[dict]:
    if not path.exists():
        print(f"[warn] {path} not found — skipping", file=sys.stderr)
        return []
    with path.open(newline="") as fh:
        return list(csv.DictReader(fh))


def load_timeline(path: Path):
    """Return (tick_list, {cpu_id: [pid_or_None, ...]}) from timeline.csv."""
    rows = load_csv(path)
    if not rows:
        return [], {}
    cpu_keys = [k for k in rows[0].keys() if k.startswith("cpu")]
    ticks = [int(r["tick"]) for r in rows]
    cpu_data: dict[str, list] = {k: [] for k in cpu_keys}
    for r in rows:
        for k in cpu_keys:
            val = r[k].strip()
            # "idle" → None;  "P3" → 3
            cpu_data[k].append(None if val == "idle" else int(val[1:]))
    return ticks, cpu_data


# ── Chart 1: Gantt chart ────────────────────────────────────────────────────
def plot_gantt(ticks, cpu_data, out_path: Path):
    import matplotlib.pyplot as plt
    import matplotlib.patches as mpatches

    if not ticks:
        return

    cpu_labels = sorted(cpu_data.keys())
    n_cpus = len(cpu_labels)
    fig_w = max(12, len(ticks) * 0.35)
    fig, ax = plt.subplots(figsize=(fig_w, n_cpus * 0.9 + 1.5))

    pid_set = set()
    for series in cpu_data.values():
        pid_set.update(p for p in series if p is not None)

    for row_idx, cpu_key in enumerate(cpu_labels):
        series = cpu_data[cpu_key]
        t = 0
        while t < len(series):
            pid = series[t]
            run_len = 1
            while t + run_len < len(series) and series[t + run_len] == pid:
                run_len += 1
            if pid is not None:
                ax.barh(
                    y=row_idx,
                    width=run_len,
                    left=ticks[t],
                    height=0.65,
                    color=colour(pid),
                    edgecolor="white",
                    linewidth=0.5,
                )
                if run_len >= 2:
                    ax.text(
                        ticks[t] + run_len / 2,
                        row_idx,
                        f"P{pid}",
                        ha="center",
                        va="center",
                        fontsize=7,
                        color="white",
                        fontweight="bold",
                    )
            t += run_len

    ax.set_yticks(range(n_cpus))
    ax.set_yticklabels([f"CPU {k.replace('cpu','')}" for k in cpu_labels])
    ax.set_xlabel("Simulation Tick")
    ax.set_title("MLFQ — Gantt Chart (per-CPU schedule)", fontweight="bold")
    ax.set_xlim(0, ticks[-1] + 1)
    ax.grid(axis="x", linestyle="--", alpha=0.4)

    # Legend
    patches = [
        mpatches.Patch(color=colour(p), label=f"P{p}") for p in sorted(pid_set)
    ]
    ax.legend(handles=patches, loc="upper right", fontsize=8,
              ncol=max(1, len(pid_set) // 4))

    fig.tight_layout()
    fig.savefig(out_path, dpi=150)
    plt.close(fig)
    print(f"saved {out_path}")


# ── Chart 2: MLFQ metrics bar ───────────────────────────────────────────────
def plot_mlfq_metrics(metrics_rows: list[dict], out_path: Path):
    import matplotlib.pyplot as plt
    import numpy as np

    if not metrics_rows:
        return

    row = metrics_rows[0]
    labels  = ["Avg Waiting", "Avg Turnaround", "Avg Response"]
    values  = [
        float(row["avg_waiting"]),
        float(row["avg_turnaround"]),
        float(row["avg_response"]),
    ]
    clrs = ["#4e79a7", "#f28e2b", "#e15759"]

    fig, ax = plt.subplots(figsize=(7, 4))
    bars = ax.bar(labels, values, color=clrs, edgecolor="white", width=0.5)
    for bar, val in zip(bars, values):
        ax.text(
            bar.get_x() + bar.get_width() / 2,
            bar.get_height() + 0.3,
            f"{val:.2f}",
            ha="center",
            va="bottom",
            fontsize=10,
            fontweight="bold",
        )
    ax.set_ylabel("Ticks")
    ax.set_title(
        f"MLFQ — Performance Metrics\n"
        f"(Throughput={float(row['throughput']):.4f}, "
        f"CPU util={float(row['cpu_utilization']):.1%})",
        fontweight="bold",
    )
    ax.set_ylim(0, max(values) * 1.25)
    ax.grid(axis="y", linestyle="--", alpha=0.4)
    fig.tight_layout()
    fig.savefig(out_path, dpi=150)
    plt.close(fig)
    print(f"saved {out_path}")


# ── Chart 3: Comparison bar chart ───────────────────────────────────────────
def plot_comparison(cmp_rows: list[dict], out_path: Path):
    import matplotlib.pyplot as plt
    import numpy as np

    if not cmp_rows:
        return

    algorithms  = [r["algorithm"] for r in cmp_rows]
    metrics     = ["avg_waiting", "avg_turnaround", "avg_response"]
    titles      = ["Avg Waiting", "Avg Turnaround", "Avg Response"]
    bar_colors  = ["#4e79a7", "#f28e2b", "#e15759", "#76b7b2"]

    x = np.arange(len(algorithms))
    width = 0.22
    n_m = len(metrics)

    fig, ax = plt.subplots(figsize=(9, 4))
    for i, (m, t) in enumerate(zip(metrics, titles)):
        vals = [float(r[m]) for r in cmp_rows]
        offset = (i - n_m / 2 + 0.5) * width
        ax.bar(x + offset, vals, width, label=t,
               color=bar_colors[i], edgecolor="white")

    ax.set_xticks(x)
    ax.set_xticklabels(algorithms, fontsize=9)
    ax.set_ylabel("Ticks")
    ax.set_title("Algorithm Comparison — Waiting / Turnaround / Response",
                 fontweight="bold")
    ax.legend(fontsize=8)
    ax.grid(axis="y", linestyle="--", alpha=0.4)
    fig.tight_layout()
    fig.savefig(out_path, dpi=150)
    plt.close(fig)
    print(f"saved {out_path}")


# ── Chart 4: CPU utilisation pie ────────────────────────────────────────────
def plot_cpu_util(metrics_rows: list[dict], out_path: Path):
    import matplotlib.pyplot as plt

    if not metrics_rows:
        return

    row  = metrics_rows[0]
    util = float(row["cpu_utilization"])
    idle = 1.0 - util

    fig, ax = plt.subplots(figsize=(5, 4))
    wedge_props = {"edgecolor": "white", "linewidth": 2}
    ax.pie(
        [util, idle],
        labels=[f"Busy\n{util:.1%}", f"Idle\n{idle:.1%}"],
        colors=["#4e79a7", "#d3d3d3"],
        autopct="%1.1f%%",
        startangle=90,
        wedgeprops=wedge_props,
        textprops={"fontsize": 11},
    )
    ax.set_title("MLFQ — CPU Utilisation", fontweight="bold")
    fig.tight_layout()
    fig.savefig(out_path, dpi=150)
    plt.close(fig)
    print(f"saved {out_path}")


# ── main ─────────────────────────────────────────────────────────────────────
def main() -> int:
    if len(sys.argv) != 2:
        print("usage: plot_mlfq.py <output_dir>", file=sys.stderr)
        print("  e.g. plot_mlfq.py out/mlfq_run", file=sys.stderr)
        return 1

    out_dir  = Path(sys.argv[1])
    mlfq_dir = out_dir / "mlfq"

    try:
        import matplotlib
        matplotlib.use("Agg")   # non-interactive backend
    except ModuleNotFoundError:
        print("matplotlib not found. Run: pip install matplotlib", file=sys.stderr)
        return 1

    # Load data
    ticks, cpu_data = load_timeline(mlfq_dir / "timeline.csv")
    metrics_rows    = load_csv(mlfq_dir / "metrics.csv")
    cmp_rows        = load_csv(out_dir / "comparison.csv")

    # Produce charts
    plot_gantt(ticks, cpu_data, out_dir / "mlfq_gantt.png")
    plot_mlfq_metrics(metrics_rows, out_dir / "mlfq_metrics_bar.png")
    plot_comparison(cmp_rows, out_dir / "mlfq_comparison.png")
    plot_cpu_util(metrics_rows, out_dir / "mlfq_cpu_util.png")

    print("\nAll charts saved to:", out_dir)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
