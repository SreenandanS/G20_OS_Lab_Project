# G20 Project 2: Advanced Scheduling Algorithms

This project is a lightweight multiprocessor scheduling simulator written in C. It compares multiple policies over the same workload file and generates CSV output for later graphing.

## Supported Algorithms

- `mlfq`
- `lottery`
- `edf`
- `custom`
- `all`

## CLI

```bash
./bin/scheduler_sim --algo {mlfq,lottery,edf,custom,all} --cpus N --input <workload.csv> --output-dir <dir>
```

## Workload Format

CSV header:

```text
pid,arrival,bursts,base_priority,tickets,deadline,affinity
```

Example burst script:

```text
C5:I3:C4:I2:C6
```

- `C<number>` is a CPU burst
- `I<number>` is an I/O wait burst
- `affinity` uses `-1` for no preferred CPU

## Build

```bash
make
./bin/scheduler_sim --algo custom --cpus 4 --input data/workloads/sample_mixed.csv --output-dir out/sample_run
```

## Graphing

```bash
python3 scripts/plot_metrics.py out/sample_run/comparison.csv
```

If `matplotlib` is missing:

```bash
python3 -m pip install matplotlib
```
