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

static int effective_deadline(const ProcessRuntime *proc)
{
  if (proc->spec.deadline >= 0) {
    return proc->spec.deadline;
  }
  return INT_MAX;
}

static int edf_is_higher_priority(const ProcessRuntime *a,
                                  const ProcessRuntime *b)
{
  int da = effective_deadline(a);
  int db = effective_deadline(b);

  if (da != db) {
    return da < db;
  }
  if (a->spec.arrival != b->spec.arrival) {
    return a->spec.arrival < b->spec.arrival;
  }
  return a->spec.pid < b->spec.pid;
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
  int i;

  for (i = 0; i < scheduler->state.edf.ready.count; i++) {
    int pid = scheduler->state.edf.ready.items[i];
    if (best_index < 0 ||
        edf_is_higher_priority(
            &procs[pid],
            &procs[scheduler->state.edf.ready.items[best_index]])) {
      best_index = i;
    }
  }

  return queue_remove_at(&scheduler->state.edf.ready, best_index);
}
