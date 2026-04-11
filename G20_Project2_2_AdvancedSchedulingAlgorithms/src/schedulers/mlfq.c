/* =============================================================================
 * mlfq.c  —  Multi-Level Feedback Queue (MLFQ) Scheduler
 * Author  : Sukrat Singh Kushwaha
 * Branch  : sukrat/threads-mlfq
 *
 * Algorithm overview
 * ------------------
 *  Three priority queues, numbered 0 (highest) to 2 (lowest):
 *
 *    Q0  (Interactive)  :  quantum =  2 ticks
 *    Q1  (Normal)       :  quantum =  4 ticks
 *    Q2  (Batch/CPU)    :  quantum =  8 ticks
 *
 *  Scheduling rules
 *  ----------------
 *  Rule 1 – Arrival:
 *    Every new process starts in Q0 (highest priority).
 *
 *  Rule 2 – CPU-bound demotion (ENQUEUE_PREEMPT):
 *    If a process uses its full quantum without blocking on I/O it is
 *    moved DOWN one level on the next enqueue.  A process that keeps
 *    burning CPU eventually settles in Q2 where it gets the longest
 *    quantum and the lowest priority.
 *
 *  Rule 3 – I/O-bound promotion (ENQUEUE_IO_COMPLETE):
 *    If a process blocks for I/O before the quantum expires it is
 *    treated as interactive and moved UP one level (toward Q0) when
 *    the I/O finishes.  This keeps latency-sensitive processes
 *    responsive.
 *
 *  Rule 4 – Periodic priority boost (every MLFQ_BOOST_TICKS ticks):
 *    All processes in Q1 and Q2 are unconditionally reset to Q0.
 *    This prevents indefinite starvation of CPU-bound jobs when the
 *    system is saturated with short interactive jobs.
 *
 * Data structures
 * ---------------
 *  Each queue is a plain fixed-size array (items[MAX_PROCESSES]) used
 *  as a FIFO.  push appends to the tail; pop removes from the head by
 *  shifting down.  No dynamic memory allocation is used.
 *
 * Metrics captured per scheduling event (printed to stdout)
 * ----------------------------------------------------------
 *    - Current tick
 *    - Which process is being enqueued and why
 *    - The target queue level after promotion/demotion
 *    - Length of all three queues at that moment
 *    - Priority boosts as they happen
 * ============================================================================= */

#include "scheduler.h"

#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------
 * MLFQ constants — all policy knobs live here, not in core files
 * ------------------------------------------------------------------ */
#define MLFQ_QUEUES         3   /* total number of priority levels           */
#define MLFQ_Q0_QUANTUM     2   /* time slice for Q0 (interactive)  [ticks] */
#define MLFQ_Q1_QUANTUM     4   /* time slice for Q1 (normal)       [ticks] */
#define MLFQ_Q2_QUANTUM     8   /* time slice for Q2 (batch/CPU)    [ticks] */
#define MLFQ_BOOST_TICKS   20   /* priority boost period            [ticks] */

/* ------------------------------------------------------------------
 * queue_push — append pid to the tail of a FIFO ready queue
 * ------------------------------------------------------------------ */
static void queue_push(ReadyQueue *q, int pid)
{
  if (q->count < MAX_PROCESSES) {
    q->items[q->count++] = pid;
  }
}

/* ------------------------------------------------------------------
 * queue_pop — remove and return the head of the FIFO ready queue.
 * Returns -1 if the queue is empty.
 * The remaining elements are shifted left by one position so the
 * array stays a contiguous FIFO (O(n), acceptable for MAX_PROCESSES).
 * ------------------------------------------------------------------ */
static int queue_pop(ReadyQueue *q)
{
  int pid;
  int i;

  if (q->count == 0) {
    return -1;
  }

  pid = q->items[0];
  for (i = 1; i < q->count; i++) {
    q->items[i - 1] = q->items[i];
  }
  q->count--;
  return pid;
}

/* ------------------------------------------------------------------
 * queue_clear — zero-fill a ReadyQueue
 * ------------------------------------------------------------------ */
static void queue_clear(ReadyQueue *q)
{
  memset(q, 0, sizeof(*q));
}

/* ------------------------------------------------------------------
 * mlfq_print_queues — helper that prints a compact queue-depth bar
 * for the three MLFQ queues, useful for visual debugging.
 *
 * Example output:   [Q0:3] [Q1:1] [Q2:0]
 * ------------------------------------------------------------------ */
static void mlfq_print_queues(const SchedulerState *s)
{
  fprintf(stdout, "[Q0:%d] [Q1:%d] [Q2:%d]",
          s->state.mlfq.queues[0].count,
          s->state.mlfq.queues[1].count,
          s->state.mlfq.queues[2].count);
}

/* ------------------------------------------------------------------
 * mlfq_init — set up all three queues and the boost clock.
 *
 * Called once by scheduler_init() before the simulation loop starts.
 * ------------------------------------------------------------------ */
void mlfq_init(SchedulerState *scheduler)
{
  int i;

  scheduler->state.mlfq.boost_interval  = MLFQ_BOOST_TICKS;
  scheduler->state.mlfq.last_boost_tick = 0;

  for (i = 0; i < MLFQ_QUEUES; i++) {
    queue_clear(&scheduler->state.mlfq.queues[i]);
  }

  fprintf(stdout,
          "[MLFQ] Init: Q0(quantum=%d) | Q1(quantum=%d) | Q2(quantum=%d)"
          " | boost_every=%d ticks\n",
          MLFQ_Q0_QUANTUM, MLFQ_Q1_QUANTUM, MLFQ_Q2_QUANTUM,
          MLFQ_BOOST_TICKS);
}

/* ------------------------------------------------------------------
 * mlfq_enqueue — place process pid in the correct priority queue.
 *
 * The queue level is determined by the current level stored in
 * procs[pid].queue_level and the reason for enqueueing:
 *
 *   ENQUEUE_NEW         → Rule 1: always Q0
 *   ENQUEUE_IO_COMPLETE → Rule 3: promote by 1 (floor at Q0)
 *   ENQUEUE_PREEMPT     → Rule 2: demote  by 1 (ceil at Q2)
 *
 * After updating the level the pid is pushed to the tail of that
 * queue's array and the new queue depths are printed.
 * ------------------------------------------------------------------ */
void mlfq_enqueue(SchedulerState *scheduler, ProcessRuntime procs[], int pid,
                  int now, EnqueueReason reason)
{
  int level = procs[pid].queue_level;
  const char *tag;

  switch (reason) {
  case ENQUEUE_NEW:
    /* Rule 1: fresh arrival always gets highest priority */
    level = 0;
    tag   = "ARRIVE ";
    break;

  case ENQUEUE_IO_COMPLETE:
    /* Rule 3: reward I/O behaviour — promote toward Q0 */
    if (level > 0) {
      level--;
    }
    tag = "IO_DONE";
    break;

  case ENQUEUE_PREEMPT:
    /* Rule 2: penalise full-quantum use — demote toward Q2 */
    if (level < MLFQ_QUEUES - 1) {
      level++;
    }
    tag = "PREEMPT";
    break;

  default:
    tag = "UNKNOWN";
    break;
  }

  procs[pid].queue_level = level;
  queue_push(&scheduler->state.mlfq.queues[level], pid);

  fprintf(stdout,
          "[MLFQ] t=%-3d  enqueue P%-2d  %-7s  -> Q%d  depths: ",
          now, procs[pid].spec.pid, tag, level);
  mlfq_print_queues(scheduler);
  fprintf(stdout, "\n");
}

/* ------------------------------------------------------------------
 * mlfq_pick_next — pick the highest-priority ready process.
 *
 * Scans Q0 → Q1 → Q2 and pops the head of the first non-empty queue.
 * Returns -1 if all queues are empty (CPU goes idle).
 *
 * The quantum for the chosen process is read from procs[pid].queue_level
 * by time_slice_for() inside simulator.c:
 *
 *   level 0 → MLFQ_Q0_QUANTUM = 2
 *   level 1 → MLFQ_Q1_QUANTUM = 4
 *   level 2 → MLFQ_Q2_QUANTUM = 8
 * ------------------------------------------------------------------ */
int mlfq_pick_next(SchedulerState *scheduler)
{
  int level;
  int pid;

  for (level = 0; level < MLFQ_QUEUES; level++) {
    if (scheduler->state.mlfq.queues[level].count > 0) {
      pid = queue_pop(&scheduler->state.mlfq.queues[level]);
      fprintf(stdout,
              "[MLFQ]          dispatch  P%-2d  from Q%d  (quantum=%d)\n",
              pid,   /* array index — matches the pid used everywhere else */
              level,
              level == 0 ? MLFQ_Q0_QUANTUM :
              level == 1 ? MLFQ_Q1_QUANTUM : MLFQ_Q2_QUANTUM);
      return pid;
    }
  }
  return -1;  /* all queues empty — CPU idles */
}

/* ------------------------------------------------------------------
 * mlfq_tick — periodic priority boost (Rule 4, anti-starvation).
 *
 * Called once per simulated tick by scheduler_tick().
 *
 * When (now - last_boost_tick) >= boost_interval, every process
 * currently waiting in Q1 or Q2 is moved back to Q0.  This ensures
 * that long-running CPU-bound jobs eventually get CPU time even when
 * the system is flooded with short interactive work.
 *
 * The boost is skipped at tick 0 to avoid an off-by-one artefact.
 * ------------------------------------------------------------------ */
void mlfq_tick(SchedulerState *scheduler, ProcessRuntime procs[], int now)
{
  int pid;
  int level;
  int boosted = 0;

  /* Not yet time for a boost */
  if (now == 0 ||
      (now - scheduler->state.mlfq.last_boost_tick) <
          scheduler->state.mlfq.boost_interval) {
    return;
  }

  /* ---- Priority Boost ---- */
  fprintf(stdout,
          "[MLFQ] t=%-3d  *** PRIORITY BOOST ***  (period=%d, prev=%d)  ",
          now,
          scheduler->state.mlfq.boost_interval,
          scheduler->state.mlfq.last_boost_tick);
  mlfq_print_queues(scheduler);
  fprintf(stdout, "\n");

  for (level = 1; level < MLFQ_QUEUES; level++) {
    while (scheduler->state.mlfq.queues[level].count > 0) {
      pid = queue_pop(&scheduler->state.mlfq.queues[level]);
      procs[pid].queue_level = 0;
      queue_push(&scheduler->state.mlfq.queues[0], pid);
      boosted++;
      fprintf(stdout,
              "[MLFQ]         boosted   P%-2d  Q%d -> Q0\n",
              procs[pid].spec.pid, level);
    }
  }

  if (boosted == 0) {
    fprintf(stdout, "[MLFQ]         (no processes waiting in Q1/Q2)\n");
  } else {
    fprintf(stdout, "[MLFQ]         after boost: ");
    mlfq_print_queues(scheduler);
    fprintf(stdout, "\n");
  }

  scheduler->state.mlfq.last_boost_tick = now;
}
