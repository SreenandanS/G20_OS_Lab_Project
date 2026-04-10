#ifndef G20_SCHEDULER_H
#define G20_SCHEDULER_H

#include "sim.h"

typedef struct {
  int items[MAX_PROCESSES];
  int count;
} ReadyQueue;

typedef struct {
  AlgorithmKind algorithm;
  int cpu_count;
  int proc_count;
  unsigned seed;
  union {
    struct {
      ReadyQueue queues[3];
      int boost_interval;
      int last_boost_tick;
    } mlfq;
    struct {
      ReadyQueue ready;
      unsigned int rng_state;
    } lottery;
    struct {
      ReadyQueue ready;
    } edf;
    struct {
      ReadyQueue queues[MAX_CPUS];
      int steal_interval;
      int last_steal_tick;
      int imbalance_threshold;
    } custom;
  } state;
} SchedulerState;

void scheduler_init(SchedulerState *scheduler, AlgorithmKind algorithm,
                    int cpu_count, int proc_count, unsigned seed);
void scheduler_enqueue(SchedulerState *scheduler, ProcessRuntime procs[],
                       int pid, int cpu_hint, int now, EnqueueReason reason);
int scheduler_pick_next(SchedulerState *scheduler, ProcessRuntime procs[],
                        int cpu_id, int now);
void scheduler_tick(SchedulerState *scheduler, ProcessRuntime procs[], int now);
int scheduler_time_slice(const SchedulerState *scheduler,
                         const ProcessRuntime *proc);

#endif
