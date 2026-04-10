#include "sim.h"
#include "scheduler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mlfq_init(SchedulerState *scheduler);
void mlfq_enqueue(SchedulerState *scheduler, ProcessRuntime procs[], int pid,
                  int now, EnqueueReason reason);
int mlfq_pick_next(SchedulerState *scheduler);
void mlfq_tick(SchedulerState *scheduler, ProcessRuntime procs[], int now);

void lottery_init(SchedulerState *scheduler);
void lottery_enqueue(SchedulerState *scheduler, int pid);
int lottery_pick_next(SchedulerState *scheduler, ProcessRuntime procs[]);

void edf_init(SchedulerState *scheduler);
void edf_enqueue(SchedulerState *scheduler, int pid);
int edf_pick_next(SchedulerState *scheduler, ProcessRuntime procs[]);

void custom_init(SchedulerState *scheduler);
void custom_enqueue(SchedulerState *scheduler, ProcessRuntime procs[], int pid,
                    int cpu_hint);
int custom_pick_next(SchedulerState *scheduler, ProcessRuntime procs[], int cpu_id,
                     int now);
void custom_tick(SchedulerState *scheduler, ProcessRuntime procs[], int now);

static int all_done(const ProcessRuntime procs[], int count)
{
  int i;
  for (i = 0; i < count; i++) {
    if (procs[i].state != PROC_DONE) {
      return 0;
    }
  }
  return 1;
}

static void record_timeline(Timeline *timeline, const CpuState cpus[],
                            const ProcessRuntime procs[], int tick)
{
  int cpu;
  for (cpu = 0; cpu < timeline->cpu_count; cpu++) {
    int slot = cpus[cpu].running_pid;
    if (slot >= 0) {
      timeline->slots[(tick * timeline->cpu_count) + cpu] = procs[slot].spec.pid;
    } else {
      timeline->slots[(tick * timeline->cpu_count) + cpu] = -1;
    }
  }
}

static void enqueue_process(SchedulerState *scheduler, ProcessRuntime procs[], int pid,
                            int cpu_hint, int now, EnqueueReason reason)
{
  procs[pid].state = PROC_READY;
  procs[pid].ready_since = now;
  if (scheduler->algorithm == ALGO_MLFQ) {
    mlfq_enqueue(scheduler, procs, pid, now, reason);
  } else if (scheduler->algorithm == ALGO_LOTTERY) {
    lottery_enqueue(scheduler, pid);
  } else if (scheduler->algorithm == ALGO_EDF) {
    edf_enqueue(scheduler, pid);
  } else {
    custom_enqueue(scheduler, procs, pid, cpu_hint);
  }
}

static int pick_next_process(SchedulerState *scheduler, ProcessRuntime procs[],
                             int cpu_id, int now)
{
  if (scheduler->algorithm == ALGO_MLFQ) {
    return mlfq_pick_next(scheduler);
  }
  if (scheduler->algorithm == ALGO_LOTTERY) {
    return lottery_pick_next(scheduler, procs);
  }
  if (scheduler->algorithm == ALGO_EDF) {
    return edf_pick_next(scheduler, procs);
  }
  return custom_pick_next(scheduler, procs, cpu_id, now);
}

static int time_slice_for(const SchedulerState *scheduler, const ProcessRuntime *proc)
{
  switch (scheduler->algorithm) {
  case ALGO_MLFQ:
    switch (proc->queue_level) {
    case 0:
      return 2;
    case 1:
      return 4;
    default:
      return 8;
    }
  case ALGO_LOTTERY:
    return 2;
  case ALGO_EDF:
    return 1;
  case ALGO_CUSTOM:
    return 4;
  case ALGO_ALL:
    break;
  }
  return 1;
}

void scheduler_init(SchedulerState *scheduler, AlgorithmKind algorithm,
                    int cpu_count, int proc_count, unsigned seed)
{
  memset(scheduler, 0, sizeof(*scheduler));
  scheduler->algorithm = algorithm;
  scheduler->cpu_count = cpu_count;
  scheduler->proc_count = proc_count;
  scheduler->seed = seed;

  if (algorithm == ALGO_MLFQ) {
    mlfq_init(scheduler);
  } else if (algorithm == ALGO_LOTTERY) {
    lottery_init(scheduler);
  } else if (algorithm == ALGO_EDF) {
    edf_init(scheduler);
  } else {
    custom_init(scheduler);
  }
}

void scheduler_enqueue(SchedulerState *scheduler, ProcessRuntime procs[], int pid,
                       int cpu_hint, int now, EnqueueReason reason)
{
  enqueue_process(scheduler, procs, pid, cpu_hint, now, reason);
}

int scheduler_pick_next(SchedulerState *scheduler, ProcessRuntime procs[],
                        int cpu_id, int now)
{
  return pick_next_process(scheduler, procs, cpu_id, now);
}

void scheduler_tick(SchedulerState *scheduler, ProcessRuntime procs[], int now)
{
  if (scheduler->algorithm == ALGO_MLFQ) {
    mlfq_tick(scheduler, procs, now);
  } else if (scheduler->algorithm == ALGO_CUSTOM) {
    custom_tick(scheduler, procs, now);
  }
}

int scheduler_time_slice(const SchedulerState *scheduler,
                         const ProcessRuntime *proc)
{
  return time_slice_for(scheduler, proc);
}

static void initialize_runtime(const Workload *workload, ProcessRuntime procs[],
                               int count)
{
  int i;

  memset(procs, 0, sizeof(ProcessRuntime) * (size_t)count);
  for (i = 0; i < count; i++) {
    procs[i].spec = workload->processes[i];
    procs[i].state = PROC_NEW;
    procs[i].burst_index = 0;
    procs[i].remaining_in_burst = workload->processes[i].bursts[0].duration;
    procs[i].ready_since = workload->processes[i].arrival;
    procs[i].first_run_tick = -1;
    procs[i].completion_tick = -1;
    procs[i].last_cpu = -1;
    procs[i].assigned_cpu = workload->processes[i].affinity;
    procs[i].queue_level = 0;
  }
}

static void initialize_cpus(CpuState cpus[], int cpu_count)
{
  int cpu;
  for (cpu = 0; cpu < cpu_count; cpu++) {
    cpus[cpu].running_pid = -1;
    cpus[cpu].slice_used = 0;
  }
}

static void collect_metrics(SimulationResult *result, const ProcessRuntime procs[],
                            int count, int total_ticks, int cpu_count,
                            int busy_ticks)
{
  int i;
  double total_waiting = 0.0;
  double total_turnaround = 0.0;
  double total_response = 0.0;
  int deadline_misses = 0;
  int migrations = 0;

  for (i = 0; i < count; i++) {
    total_waiting += procs[i].total_waiting;
    total_turnaround += procs[i].completion_tick - procs[i].spec.arrival;
    total_response += procs[i].first_run_tick - procs[i].spec.arrival;
    if (procs[i].spec.deadline >= 0 && procs[i].completion_tick > procs[i].spec.deadline) {
      deadline_misses++;
    }
    migrations += procs[i].migrations;
  }

  result->metrics.total_ticks = total_ticks;
  result->metrics.completed = count;
  result->metrics.deadline_misses = deadline_misses;
  result->metrics.busy_ticks = busy_ticks;
  result->metrics.migrations = migrations;
  result->metrics.avg_waiting = total_waiting / count;
  result->metrics.avg_turnaround = total_turnaround / count;
  result->metrics.avg_response = total_response / count;
  result->metrics.throughput = count / (double)(total_ticks > 0 ? total_ticks : 1);
  result->metrics.cpu_utilization =
      busy_ticks / (double)((total_ticks > 0 ? total_ticks : 1) * cpu_count);
}

int run_simulation(const Workload *workload, const SimulationConfig *config,
                   AlgorithmKind algorithm, SimulationResult *result)
{
  SchedulerState scheduler;
  ProcessRuntime procs[MAX_PROCESSES];
  CpuState cpus[MAX_CPUS];
  int tick;
  int busy_ticks = 0;
  int i;

  memset(result, 0, sizeof(*result));
  result->metrics.algorithm = algorithm;
  result->metrics.algorithm_name = algorithm_kind_name(algorithm);
  result->metrics.cpu_count = config->cpu_count;
  result->metrics.seed = config->seed;

  initialize_runtime(workload, procs, workload->process_count);
  initialize_cpus(cpus, config->cpu_count);
  scheduler_init(&scheduler, algorithm, config->cpu_count, workload->process_count,
                 config->seed);

  result->timeline.cpu_count = config->cpu_count;
  result->timeline.tick_count = config->max_ticks;
  result->timeline.slots =
      malloc(sizeof(int) * (size_t)config->cpu_count * (size_t)config->max_ticks);
  if (result->timeline.slots == NULL) {
    return -1;
  }
  for (i = 0; i < config->cpu_count * config->max_ticks; i++) {
    result->timeline.slots[i] = -1;
  }

  for (tick = 0; tick < config->max_ticks; tick++) {
    for (i = 0; i < workload->process_count; i++) {
      if (procs[i].state == PROC_NEW && procs[i].spec.arrival <= tick) {
        scheduler_enqueue(&scheduler, procs, i, procs[i].assigned_cpu, tick,
                          ENQUEUE_NEW);
      }
    }

    for (i = 0; i < workload->process_count; i++) {
      if (procs[i].state == PROC_BLOCKED) {
        procs[i].remaining_in_burst--;
        if (procs[i].remaining_in_burst <= 0) {
          procs[i].burst_index++;
          if (procs[i].burst_index >= procs[i].spec.burst_count) {
            procs[i].state = PROC_DONE;
            procs[i].completion_tick = tick;
          } else {
            procs[i].remaining_in_burst =
                procs[i].spec.bursts[procs[i].burst_index].duration;
            scheduler_enqueue(&scheduler, procs, i, procs[i].assigned_cpu, tick,
                              ENQUEUE_IO_COMPLETE);
          }
        }
      }
    }

    scheduler_tick(&scheduler, procs, tick);

    for (i = 0; i < workload->process_count; i++) {
      if (procs[i].state == PROC_READY) {
        procs[i].total_waiting++;
      }
    }

    for (i = 0; i < config->cpu_count; i++) {
      if (cpus[i].running_pid < 0) {
        int pid = scheduler_pick_next(&scheduler, procs, i, tick);
        if (pid >= 0) {
          ProcessRuntime *proc = &procs[pid];
          proc->state = PROC_RUNNING;
          if (proc->first_run_tick < 0) {
            proc->first_run_tick = tick;
          }
          if (proc->last_cpu >= 0 && proc->last_cpu != i) {
            proc->migrations++;
          }
          proc->last_cpu = i;
          cpus[i].running_pid = pid;
          cpus[i].slice_used = 0;
        }
      }
    }

    record_timeline(&result->timeline, cpus, procs, tick);

    for (i = 0; i < config->cpu_count; i++) {
      int pid = cpus[i].running_pid;
      int slice;
      ProcessRuntime *proc;

      if (pid < 0) {
        continue;
      }

      proc = &procs[pid];
      busy_ticks++;
      proc->runtime_ticks++;
      proc->remaining_in_burst--;
      cpus[i].slice_used++;

      if (proc->remaining_in_burst <= 0) {
        proc->burst_index++;
        if (proc->burst_index >= proc->spec.burst_count) {
          proc->state = PROC_DONE;
          proc->completion_tick = tick + 1;
        } else {
          proc->state = PROC_BLOCKED;
          proc->remaining_in_burst =
              proc->spec.bursts[proc->burst_index].duration;
        }
        cpus[i].running_pid = -1;
        cpus[i].slice_used = 0;
        continue;
      }

      slice = scheduler_time_slice(&scheduler, proc);
      if (cpus[i].slice_used >= slice) {
        scheduler_enqueue(&scheduler, procs, pid, i, tick + 1, ENQUEUE_PREEMPT);
        cpus[i].running_pid = -1;
        cpus[i].slice_used = 0;
      }
    }

    if (all_done(procs, workload->process_count)) {
      result->timeline.tick_count = tick + 1;
      collect_metrics(result, procs, workload->process_count, tick + 1,
                      config->cpu_count, busy_ticks);
      return 0;
    }
  }

  result->timeline.tick_count = config->max_ticks;
  collect_metrics(result, procs, workload->process_count, config->max_ticks,
                  config->cpu_count, busy_ticks);
  return 0;
}
