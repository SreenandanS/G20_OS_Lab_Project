# Metrics Validation Plan

The simulator exports enough metrics to compare fairness, responsiveness, and multiprocessor balancing.

## Metrics

- average waiting time
- average turnaround time
- average response time
- throughput
- CPU utilization
- deadline misses
- migrations

## Baseline Validation Expectations

- `mlfq` should favor interactive jobs through higher-priority queues
- `lottery` should reflect ticket proportions over longer runs
- `edf` should minimize misses on feasible deadline-heavy workloads
- `custom` should reduce queue imbalance and preserve affinity where practical
