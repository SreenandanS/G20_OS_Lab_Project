# Metrics Validation

This note validates that documented metrics match generated CSV output from the simulator.

## Repro Command

```bash
make
./bin/scheduler_sim --algo all --cpus 4 --input data/workloads/sample_mixed.csv --output-dir out/all_run
./bin/scheduler_sim --algo all --cpus 4 --input data/workloads/interactive_bursty.csv --output-dir out/interactive_bursty_run
./bin/scheduler_sim --algo all --cpus 4 --input data/workloads/lottery_ticket_skew.csv --output-dir out/lottery_ticket_skew_run
./bin/scheduler_sim --algo all --cpus 4 --input data/workloads/edf_deadline_pressure.csv --output-dir out/edf_deadline_pressure_run
```

## Workloads And Intended Behavior

- sample_mixed.csv: baseline mixed arrival and burst pattern.
- interactive_bursty.csv: short CPU bursts separated by I/O to highlight responsiveness behavior.
- lottery_ticket_skew.csv: heavy ticket skew to expose proportional-share behavior.
- edf_deadline_pressure.csv: tighter deadlines with mixed burst complexity to stress deadline handling.

## CSV-Verified Metrics

Values below are copied from generated comparison files.

### sample_mixed
Source: out/all_run/comparison.csv

| algorithm | cpu_count | total_ticks | completed | avg_waiting | avg_turnaround | avg_response | throughput | cpu_utilization | deadline_misses | migrations |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| mlfq | 4 | 11 | 4 | 3.50 | 8.50 | 0.00 | 0.3636 | 0.7045 | 0 | 5 |
| lottery | 4 | 11 | 4 | 4.25 | 8.50 | 0.00 | 0.3636 | 0.7045 | 0 | 4 |
| edf | 4 | 11 | 4 | 7.75 | 8.50 | 0.00 | 0.3636 | 0.7045 | 0 | 6 |
| custom | 4 | 16 | 4 | 6.75 | 12.50 | 0.75 | 0.2500 | 0.4844 | 1 | 2 |

### interactive_bursty
Source: out/interactive_bursty_run/comparison.csv

| algorithm | cpu_count | total_ticks | completed | avg_waiting | avg_turnaround | avg_response | throughput | cpu_utilization | deadline_misses | migrations |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| mlfq | 4 | 16 | 5 | 4.60 | 13.00 | 0.00 | 0.3125 | 0.7969 | 0 | 12 |
| lottery | 4 | 17 | 5 | 6.00 | 13.00 | 0.00 | 0.2941 | 0.7500 | 0 | 16 |
| edf | 4 | 17 | 5 | 10.80 | 13.00 | 0.00 | 0.2941 | 0.7500 | 0 | 23 |
| custom | 4 | 21 | 5 | 9.00 | 17.80 | 0.80 | 0.2381 | 0.6071 | 0 | 2 |

### lottery_ticket_skew
Source: out/lottery_ticket_skew_run/comparison.csv

| algorithm | cpu_count | total_ticks | completed | avg_waiting | avg_turnaround | avg_response | throughput | cpu_utilization | deadline_misses | migrations |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| mlfq | 4 | 28 | 5 | 7.60 | 23.00 | 0.60 | 0.1786 | 0.8750 | 0 | 10 |
| lottery | 4 | 38 | 5 | 13.20 | 23.40 | 2.60 | 0.1316 | 0.6447 | 0 | 20 |
| edf | 4 | 40 | 5 | 23.00 | 23.40 | 3.40 | 0.1250 | 0.6125 | 0 | 0 |
| custom | 4 | 32 | 5 | 8.00 | 23.00 | 0.20 | 0.1562 | 0.7656 | 0 | 1 |

### edf_deadline_pressure
Source: out/edf_deadline_pressure_run/comparison.csv

| algorithm | cpu_count | total_ticks | completed | avg_waiting | avg_turnaround | avg_response | throughput | cpu_utilization | deadline_misses | migrations |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| mlfq | 4 | 16 | 5 | 5.00 | 11.00 | 0.00 | 0.3125 | 0.7500 | 0 | 7 |
| lottery | 4 | 18 | 5 | 6.00 | 10.80 | 0.00 | 0.2778 | 0.6667 | 0 | 11 |
| edf | 4 | 19 | 5 | 10.40 | 10.80 | 0.20 | 0.2632 | 0.6316 | 0 | 14 |
| custom | 4 | 21 | 5 | 7.40 | 14.40 | 1.60 | 0.2381 | 0.5714 | 1 | 1 |

<!-- ## Viva Notes

- Why scheduler experiments need carefully designed workloads: scheduler differences only appear when workload structure stresses specific trade-offs (arrival skew, burst shape, deadlines, and affinity). Similar or random workloads can mask behavioral differences.
- Priority input vs measured performance:
  - Priority input is scheduler-facing metadata from workload or process controls (base_priority, tickets, deadlines, affinity, or priority syscalls).
  - Measured performance is observed output metrics after contention and runtime interactions (waiting, turnaround, response, throughput, utilization, misses, migrations).
  - Strong priority input does not automatically imply best measured output across all workloads. -->
