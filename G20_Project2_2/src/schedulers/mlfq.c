#include "scheduler.h"

#include <string.h>

static void queue_push(ReadyQueue *queue, int pid)
{
  if (queue->count < MAX_PROCESSES) {
    queue->items[queue->count++] = pid;
  }
}

static int queue_pop(ReadyQueue *queue)
{
  int pid;
  int i;

  if (queue->count == 0) {
    return -1;
  }

  pid = queue->items[0];
  for (i = 1; i < queue->count; i++) {
    queue->items[i - 1] = queue->items[i];
  }
  queue->count--;
  return pid;
}

static void queue_clear(ReadyQueue *queue)
{
  memset(queue, 0, sizeof(*queue));
}

void mlfq_init(SchedulerState *scheduler)
{
  int i;
  scheduler->state.mlfq.boost_interval = 20;
  scheduler->state.mlfq.last_boost_tick = 0;
  for (i = 0; i < 3; i++) {
    queue_clear(&scheduler->state.mlfq.queues[i]);
  }
}

void mlfq_enqueue(SchedulerState *scheduler, ProcessRuntime procs[], int pid,
                  int now, EnqueueReason reason)
{
  int level = procs[pid].queue_level;

  (void)now;
  if (reason == ENQUEUE_NEW) {
    level = 0;
  } else if (reason == ENQUEUE_IO_COMPLETE) {
    if (level > 0) {
      level--;
    }
  } else if (reason == ENQUEUE_PREEMPT) {
    if (level < 2) {
      level++;
    }
  }

  procs[pid].queue_level = level;
  queue_push(&scheduler->state.mlfq.queues[level], pid);
}

int mlfq_pick_next(SchedulerState *scheduler)
{
  int level;

  for (level = 0; level < 3; level++) {
    if (scheduler->state.mlfq.queues[level].count > 0) {
      return queue_pop(&scheduler->state.mlfq.queues[level]);
    }
  }
  return -1;
}

void mlfq_tick(SchedulerState *scheduler, ProcessRuntime procs[], int now)
{
  int pid;
  int level;

  if (now == 0 || (now - scheduler->state.mlfq.last_boost_tick) <
                      scheduler->state.mlfq.boost_interval) {
    return;
  }

  for (level = 1; level < 3; level++) {
    while (scheduler->state.mlfq.queues[level].count > 0) {
      pid = queue_pop(&scheduler->state.mlfq.queues[level]);
      procs[pid].queue_level = 0;
      queue_push(&scheduler->state.mlfq.queues[0], pid);
    }
  }
  scheduler->state.mlfq.last_boost_tick = now;
}
