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

void edf_init(SchedulerState *scheduler)
{
  memset(&scheduler->state.edf.ready, 0, sizeof(scheduler->state.edf.ready));
}

void edf_enqueue(SchedulerState *scheduler, int pid)
{
  queue_push(&scheduler->state.edf.ready, pid);
}

int edf_pick_next(SchedulerState *scheduler, ProcessRuntime procs[])
{
  int best_index = -1;
  int best_deadline = INT_MAX;
  int i;

  for (i = 0; i < scheduler->state.edf.ready.count; i++) {
    int pid = scheduler->state.edf.ready.items[i];
    int deadline = procs[pid].spec.deadline;
    if (deadline < 0) {
      deadline = INT_MAX - procs[pid].spec.pid;
    }

    if (best_index < 0 || deadline < best_deadline ||
        (deadline == best_deadline &&
         procs[pid].spec.arrival < procs[scheduler->state.edf.ready.items[best_index]].spec.arrival)) {
      best_index = i;
      best_deadline = deadline;
    }
  }

  return queue_remove_at(&scheduler->state.edf.ready, best_index);
}
