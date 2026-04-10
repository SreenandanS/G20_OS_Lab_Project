# G20 Operating Systems Lab Projects

This repository contains the two required submissions for the NSCS210/CSC211 Operating Systems Lab.

## Team Members

- Sreenandan Shashidharan
- Sukrat Singh Kushwaha
- Sura Manohar Sagar
- Suraj Kumar Prajapati
- Suryansh Kulshreshtha
- T Mokshitha

## Repository Layout

- `G20_Project1_xv6CustomizeSystemCalls/`: xv6-riscv customization work based on `mit-pdos/xv6-riscv`
- `G20_Project2_2_AdvancedSchedulingAlgorithms/`: multiprocessor scheduling simulator with multiple algorithms
- `TEAM_EXECUTION_GUIDE.md`: member-by-member ownership, branch plan, build steps, and integration rules

## Current Status

- Project 1 scaffolding is vendored from the xv6-riscv codebase and includes Sreenandan's signal/alarm implementation area.
- Project 2 includes the simulator foundation, a working `custom` scheduler base, and placeholders for the remaining algorithms.
- xv6-riscv validation is intentionally deferred until the project is built on an Ubuntu or lab Linux environment with the RISC-V toolchain.
- Each project README is maintained alongside the code so ownership and usage stay easy to follow.

## Quick Start

### Project 1

Project 1 must be built against the official xv6-riscv toolchain setup in an Ubuntu or lab Linux environment. Upstream xv6-riscv expects a RISC-V newlib toolchain such as `riscv64-unknown-elf-gcc` and QEMU with `riscv64-softmmu`.

Typical Linux package route:

- `build-essential`
- `bc`
- `gdb-multiarch`
- `qemu-system-misc`
- `gcc-riscv64-unknown-elf`
- `binutils-riscv64-unknown-elf`

Typical run:

```bash
cd G20_Project1_xv6CustomizeSystemCalls
make qemu
```

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
