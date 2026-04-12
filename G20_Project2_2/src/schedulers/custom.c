#include "scheduler.h"

#include <limits.h>
#include <string.h>

static void queue_push(ReadyQueue *queue, int pid)
{
  if (queue->count < MAX_PROCESSES) {
    queue->items[queue->count++] = pid;
  }
}

static int queue_remove_at(ReadyQueue *queue, int index)
{
  int pid;
  int i;

  if (index < 0 || index >= queue->count) {
    return -1;
  }

  pid = queue->items[index];
  for (i = index + 1; i < queue->count; i++) {
    queue->items[i - 1] = queue->items[i];
  }
  queue->count--;
  return pid;
}

static int least_loaded_cpu(const SchedulerState *scheduler)
{
  int cpu;
  int best_cpu = 0;
  int best_count = scheduler->state.custom.queues[0].count;

  for (cpu = 1; cpu < scheduler->cpu_count; cpu++) {
    if (scheduler->state.custom.queues[cpu].count < best_count) {
      best_count = scheduler->state.custom.queues[cpu].count;
      best_cpu = cpu;
    }
  }
  return best_cpu;
}

static int most_loaded_cpu(const SchedulerState *scheduler)
{
  int cpu;
  int best_cpu = 0;
  int best_count = scheduler->state.custom.queues[0].count;

  for (cpu = 1; cpu < scheduler->cpu_count; cpu++) {
    if (scheduler->state.custom.queues[cpu].count > best_count) {
      best_count = scheduler->state.custom.queues[cpu].count;
      best_cpu = cpu;
    }
  }
  return best_cpu;
}

void custom_init(SchedulerState *scheduler)
{
  int cpu;

  for (cpu = 0; cpu < MAX_CPUS; cpu++) {
    memset(&scheduler->state.custom.queues[cpu], 0,
           sizeof(scheduler->state.custom.queues[cpu]));
  }

  scheduler->state.custom.steal_interval = 5;
  scheduler->state.custom.last_steal_tick = 0;
  scheduler->state.custom.imbalance_threshold = 2;
}

void custom_enqueue(SchedulerState *scheduler, ProcessRuntime procs[], int pid,
                    int cpu_hint)
{
  int preferred = least_loaded_cpu(scheduler);
  int minimum_load = scheduler->state.custom.queues[preferred].count;
  int affinity = procs[pid].spec.affinity;

  if (affinity >= 0 && affinity < scheduler->cpu_count) {
    preferred = affinity;
  } else if (cpu_hint >= 0 && cpu_hint < scheduler->cpu_count) {
    preferred = cpu_hint;
  }

  if (scheduler->state.custom.queues[preferred].count > minimum_load + 1) {
    preferred = least_loaded_cpu(scheduler);
  }

  procs[pid].assigned_cpu = preferred;
  queue_push(&scheduler->state.custom.queues[preferred], pid);
}

int custom_pick_next(SchedulerState *scheduler, ProcessRuntime procs[], int cpu_id,
                     int now)
{
  ReadyQueue *queue = &scheduler->state.custom.queues[cpu_id];
  int best_index = -1;
  int best_score = INT_MAX;
  int i;

  for (i = 0; i < queue->count; i++) {
    int pid = queue->items[i];
    int age = now - procs[pid].ready_since;
    int affinity_bonus = (procs[pid].spec.affinity == cpu_id) ? -2 : 0;
    int score = procs[pid].remaining_in_burst - age + affinity_bonus;

    if (best_index < 0 || score < best_score ||
        (score == best_score && procs[pid].spec.pid < procs[queue->items[best_index]].spec.pid)) {
      best_index = i;
      best_score = score;
    }
  }

  if (best_index < 0) {
    return -1;
  }
  return queue_remove_at(queue, best_index);
}

void custom_tick(SchedulerState *scheduler, ProcessRuntime procs[], int now)
{
  int heavy_cpu;
  int light_cpu;
  int heavy_count;
  int light_count;
  int pid;

  if (now == 0 ||
      (now - scheduler->state.custom.last_steal_tick) <
          scheduler->state.custom.steal_interval) {
    return;
  }

  heavy_cpu = most_loaded_cpu(scheduler);
  light_cpu = least_loaded_cpu(scheduler);
  heavy_count = scheduler->state.custom.queues[heavy_cpu].count;
  light_count = scheduler->state.custom.queues[light_cpu].count;

  if (heavy_count - light_count < scheduler->state.custom.imbalance_threshold) {
    return;
  }

  pid = queue_remove_at(&scheduler->state.custom.queues[heavy_cpu], heavy_count - 1);
  if (pid >= 0) {
    procs[pid].assigned_cpu = light_cpu;
    queue_push(&scheduler->state.custom.queues[light_cpu], pid);
  }
  scheduler->state.custom.last_steal_tick = now;
}
