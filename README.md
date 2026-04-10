# G20 Operating Systems Lab Projects

This repository contains the two required submissions for the NSCS210/CSC211 Operating Systems Lab.

## Repository Layout

- `G20_Project1_xv6CustomizeSystemCalls/`: xv6 x86 customization work based on `mit-pdos/xv6-public`
- `G20_Project2_2_AdvancedSchedulingAlgorithms/`: multiprocessor scheduling simulator with multiple algorithms
- `TEAM_EXECUTION_GUIDE.md`: member-by-member ownership, branch plan, build steps, and integration rules

## Current Status

- Project 1 scaffolding is vendored from the xv6 x86 codebase and includes Sreenandan's signal/alarm implementation area.
- Project 2 includes the simulator foundation, a working `custom` scheduler base, and placeholders for the remaining algorithms.
- xv6 validation is intentionally deferred until the project is built on an Ubuntu or lab Linux environment with x86 tooling.

## Quick Start

### Project 1

Project 1 must be built in an Ubuntu or lab Linux environment with:

- `build-essential`
- `gdb`
- `gcc-multilib`
- `qemu-system-x86`

See `TEAM_EXECUTION_GUIDE.md` for the exact ownership split and workflow.

### Project 2

```bash
cd G20_Project2_2_AdvancedSchedulingAlgorithms
make
./bin/scheduler_sim --algo custom --cpus 4 --input data/workloads/sample_mixed.csv --output-dir out/sample_run
python3 scripts/plot_metrics.py out/sample_run/comparison.csv
```

If `matplotlib` is missing:

```bash
python3 -m pip install matplotlib
```
