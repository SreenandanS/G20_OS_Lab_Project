# G20 Project 2: Advanced Scheduling Algorithms

## Overview

This project implements a C-based multiprocessor scheduling simulator for comparing advanced scheduling policies on the same workload set. The simulator accepts workload files, runs one algorithm or all algorithms together, prints a readable schedule in the terminal, exports CSV results, and generates graphs for evaluation.

## Team Members

- Sreenandan Shashidharan
- Sukrat Singh Kushwaha
- Sura Manohar Sagar
- Suraj Kumar Prajapati
- Suryansh Kulshreshtha
- T Mokshitha

## Implemented Algorithms

- `mlfq`: Multilevel Feedback Queue with three queues and periodic priority boost
- `lottery`: ticket-based probabilistic scheduling with deterministic seeding
- `edf`: Earliest Deadline First for deadline-aware selection
- `custom`: affinity-aware multiprocessor scheduler with aging and work stealing

## Command Line Interface

```bash
./bin/scheduler_sim --algo {mlfq,lottery,edf,custom,all} --cpus N --input <workload.csv> --output-dir <dir>
```

## Workload Format

```text
pid,arrival,bursts,base_priority,tickets,deadline,affinity
```

Burst script example:

```text
C5:I3:C4:I2:C6
```

- `C<number>`: CPU burst
- `I<number>`: I/O wait burst
- `affinity`: preferred CPU id, or `-1` when no preference is supplied

## Included Workloads

- `sample_mixed.csv`
- `interactive_bursty.csv`
- `edf_deadline_pressure.csv`
- `lottery_ticket_skew.csv`
- `multiprocessor_affinity.csv`

## Build and Run

```bash
make
./bin/scheduler_sim --algo all --cpus 4 --input data/workloads/sample_mixed.csv --output-dir out/sample_run
```

## Generated Output

Each run produces:

- terminal schedule view
- per-algorithm `metrics.csv`
- per-algorithm `timeline.csv`
- per-algorithm `summary.txt`
- combined `comparison.csv`
- graph output through `scripts/plot_metrics.py`

## Additional Features

- readable ASCII schedule output for viva demonstration
- support for running all algorithms on the same workload in one command
- CPU migration and deadline-miss tracking
- affinity-aware custom scheduler
- graph generation for key performance metrics

## Documentation

Detailed discussion, workload explanations, screenshots, and graphs are available in:

- `docs/REPORT.md`
- `docs/`
