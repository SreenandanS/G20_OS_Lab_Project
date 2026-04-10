#ifndef G20_SIM_H
#define G20_SIM_H

#include <stdio.h>

#define MAX_PROCESSES 128
#define MAX_BURSTS 32
#define MAX_CPUS 8
#define MAX_PATH_LEN 512

typedef enum {
  ALGO_MLFQ = 0,
  ALGO_LOTTERY,
  ALGO_EDF,
  ALGO_CUSTOM,
  ALGO_ALL
} AlgorithmKind;

typedef enum {
  BURST_CPU = 0,
  BURST_IO
} BurstType;

typedef enum {
  PROC_NEW = 0,
  PROC_READY,
  PROC_RUNNING,
  PROC_BLOCKED,
  PROC_DONE
} ProcState;

typedef enum {
  ENQUEUE_NEW = 0,
  ENQUEUE_IO_COMPLETE,
  ENQUEUE_PREEMPT
} EnqueueReason;

typedef struct {
  BurstType type;
  int duration;
} Burst;

typedef struct {
  int pid;
  int arrival;
  int base_priority;
  int tickets;
  int deadline;
  int affinity;
  int burst_count;
  Burst bursts[MAX_BURSTS];
} ProcessSpec;

typedef struct {
  ProcessSpec spec;
  ProcState state;
  int burst_index;
  int remaining_in_burst;
  int ready_since;
  int first_run_tick;
  int completion_tick;
  int total_waiting;
  int runtime_ticks;
  int migrations;
  int last_cpu;
  int assigned_cpu;
  int queue_level;
} ProcessRuntime;

typedef struct {
  int running_pid;
  int slice_used;
} CpuState;

typedef struct {
  int tick_count;
  int cpu_count;
  int *slots;
} Timeline;

typedef struct {
  AlgorithmKind algorithm;
  const char *algorithm_name;
  int cpu_count;
  unsigned seed;
  int total_ticks;
  int completed;
  int deadline_misses;
  int busy_ticks;
  int migrations;
  double throughput;
  double cpu_utilization;
  double avg_waiting;
  double avg_turnaround;
  double avg_response;
} SimulationMetrics;

typedef struct {
  SimulationMetrics metrics;
  Timeline timeline;
} SimulationResult;

typedef struct {
  AlgorithmKind algorithm;
  int cpu_count;
  unsigned seed;
  int max_ticks;
  int run_all;
  char input_path[MAX_PATH_LEN];
  char output_dir[MAX_PATH_LEN];
} SimulationConfig;

typedef struct {
  ProcessSpec processes[MAX_PROCESSES];
  int process_count;
} Workload;

int parse_workload(const char *path, Workload *workload);
int parse_algorithm_kind(const char *value, AlgorithmKind *kind);
const char *algorithm_kind_name(AlgorithmKind kind);
int ensure_output_dir(const char *path);
int run_simulation(const Workload *workload, const SimulationConfig *config,
                   AlgorithmKind algorithm, SimulationResult *result);
int write_result_files(const char *base_dir, const char *workload_path,
                       const SimulationResult *result);
int write_comparison_csv(const char *base_dir, const SimulationResult *results,
                         int count);
void print_metrics(FILE *stream, const char *label,
                   const SimulationMetrics *metrics);
void free_result(SimulationResult *result);

#endif
