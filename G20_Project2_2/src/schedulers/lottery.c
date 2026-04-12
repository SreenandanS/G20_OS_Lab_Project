#include "scheduler.h"

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

static unsigned int next_random(unsigned int *state)
{
  *state = (*state * 1103515245u) + 12345u;
  return *state;
}

void lottery_init(SchedulerState *scheduler)
{
  memset(&scheduler->state.lottery.ready, 0, sizeof(scheduler->state.lottery.ready));
  scheduler->state.lottery.rng_state = scheduler->seed ? scheduler->seed : 42u;
}

void lottery_enqueue(SchedulerState *scheduler, int pid)
{
  queue_push(&scheduler->state.lottery.ready, pid);
}

int lottery_pick_next(SchedulerState *scheduler, ProcessRuntime procs[])
{
  int total_tickets = 0;
  int i;
  int target;
  int running = 0;

  for (i = 0; i < scheduler->state.lottery.ready.count; i++) {
    total_tickets += procs[scheduler->state.lottery.ready.items[i]].spec.tickets;
  }

  if (total_tickets <= 0) {
    return -1;
  }

  target = (int)(next_random(&scheduler->state.lottery.rng_state) % (unsigned)total_tickets);
  for (i = 0; i < scheduler->state.lottery.ready.count; i++) {
    int pid = scheduler->state.lottery.ready.items[i];
    running += procs[pid].spec.tickets;
    if (running > target) {
      return queue_remove_at(&scheduler->state.lottery.ready, i);
    }
  }

  return queue_remove_at(&scheduler->state.lottery.ready,
                         scheduler->state.lottery.ready.count - 1);
}
