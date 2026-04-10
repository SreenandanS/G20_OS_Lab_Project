#include "sim.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

static int mkdir_if_needed(const char *path)
{
  if (mkdir(path, 0777) == 0 || errno == EEXIST) {
    return 0;
  }
  return -1;
}

int ensure_output_dir(const char *path)
{
  char buffer[MAX_PATH_LEN];
  size_t i;

  if (strlen(path) >= sizeof(buffer)) {
    return -1;
  }

  snprintf(buffer, sizeof(buffer), "%s", path);
  for (i = 1; buffer[i] != '\0'; i++) {
    if (buffer[i] == '/') {
      buffer[i] = '\0';
      if (buffer[0] != '\0' && mkdir_if_needed(buffer) < 0) {
        return -1;
      }
      buffer[i] = '/';
    }
  }
  return mkdir_if_needed(buffer);
}

static int write_metrics_csv_file(const char *path, const SimulationResult *result)
{
  FILE *file = fopen(path, "w");
  if (file == NULL) {
    return -1;
  }

  fprintf(file, "algorithm,cpu_count,total_ticks,completed,avg_waiting,avg_turnaround,avg_response,throughput,cpu_utilization,deadline_misses,migrations\n");
  fprintf(file, "%s,%d,%d,%d,%.2f,%.2f,%.2f,%.4f,%.4f,%d,%d\n",
          result->metrics.algorithm_name,
          result->metrics.cpu_count,
          result->metrics.total_ticks,
          result->metrics.completed,
          result->metrics.avg_waiting,
          result->metrics.avg_turnaround,
          result->metrics.avg_response,
          result->metrics.throughput,
          result->metrics.cpu_utilization,
          result->metrics.deadline_misses,
          result->metrics.migrations);
  fclose(file);
  return 0;
}

static int write_timeline_csv_file(const char *path, const SimulationResult *result)
{
  FILE *file;
  int tick;
  int cpu;

  file = fopen(path, "w");
  if (file == NULL) {
    return -1;
  }

  fprintf(file, "tick");
  for (cpu = 0; cpu < result->timeline.cpu_count; cpu++) {
    fprintf(file, ",cpu%d", cpu);
  }
  fprintf(file, "\n");

  for (tick = 0; tick < result->timeline.tick_count; tick++) {
    fprintf(file, "%d", tick);
    for (cpu = 0; cpu < result->timeline.cpu_count; cpu++) {
      int pid = result->timeline.slots[(tick * result->timeline.cpu_count) + cpu];
      if (pid >= 0) {
        fprintf(file, ",P%d", pid);
      } else {
        fprintf(file, ",idle");
      }
    }
    fprintf(file, "\n");
  }

  fclose(file);
  return 0;
}

static int write_summary_file(const char *path, const char *workload_path,
                              const SimulationResult *result)
{
  FILE *file = fopen(path, "w");
  if (file == NULL) {
    return -1;
  }

  fprintf(file, "Algorithm: %s\n", result->metrics.algorithm_name);
  fprintf(file, "Workload: %s\n", workload_path);
  fprintf(file, "CPUs: %d\n", result->metrics.cpu_count);
  fprintf(file, "Ticks: %d\n", result->metrics.total_ticks);
  fprintf(file, "Completed: %d\n", result->metrics.completed);
  fprintf(file, "Average waiting time: %.2f\n", result->metrics.avg_waiting);
  fprintf(file, "Average turnaround time: %.2f\n", result->metrics.avg_turnaround);
  fprintf(file, "Average response time: %.2f\n", result->metrics.avg_response);
  fprintf(file, "Throughput: %.4f\n", result->metrics.throughput);
  fprintf(file, "CPU utilization: %.4f\n", result->metrics.cpu_utilization);
  fprintf(file, "Deadline misses: %d\n", result->metrics.deadline_misses);
  fprintf(file, "Migrations: %d\n", result->metrics.migrations);
  fclose(file);
  return 0;
}

int write_result_files(const char *base_dir, const char *workload_path,
                       const SimulationResult *result)
{
  char dir[MAX_PATH_LEN];
  char metrics_path[MAX_PATH_LEN];
  char timeline_path[MAX_PATH_LEN];
  char summary_path[MAX_PATH_LEN];

  snprintf(dir, sizeof(dir), "%s/%s", base_dir, result->metrics.algorithm_name);
  if (ensure_output_dir(dir) < 0) {
    return -1;
  }

  snprintf(metrics_path, sizeof(metrics_path), "%s/metrics.csv", dir);
  snprintf(timeline_path, sizeof(timeline_path), "%s/timeline.csv", dir);
  snprintf(summary_path, sizeof(summary_path), "%s/summary.txt", dir);

  if (write_metrics_csv_file(metrics_path, result) < 0 ||
      write_timeline_csv_file(timeline_path, result) < 0 ||
      write_summary_file(summary_path, workload_path, result) < 0) {
    return -1;
  }
  return 0;
}

int write_comparison_csv(const char *base_dir, const SimulationResult *results, int count)
{
  char path[MAX_PATH_LEN];
  FILE *file;
  int i;

  snprintf(path, sizeof(path), "%s/comparison.csv", base_dir);
  file = fopen(path, "w");
  if (file == NULL) {
    return -1;
  }

  fprintf(file, "algorithm,cpu_count,total_ticks,completed,avg_waiting,avg_turnaround,avg_response,throughput,cpu_utilization,deadline_misses,migrations\n");
  for (i = 0; i < count; i++) {
    fprintf(file, "%s,%d,%d,%d,%.2f,%.2f,%.2f,%.4f,%.4f,%d,%d\n",
            results[i].metrics.algorithm_name,
            results[i].metrics.cpu_count,
            results[i].metrics.total_ticks,
            results[i].metrics.completed,
            results[i].metrics.avg_waiting,
            results[i].metrics.avg_turnaround,
            results[i].metrics.avg_response,
            results[i].metrics.throughput,
            results[i].metrics.cpu_utilization,
            results[i].metrics.deadline_misses,
            results[i].metrics.migrations);
  }

  fclose(file);
  return 0;
}

void print_metrics(FILE *stream, const char *label, const SimulationMetrics *metrics)
{
  fprintf(stream,
          "%s: waiting=%.2f turnaround=%.2f response=%.2f throughput=%.4f util=%.4f misses=%d migrations=%d\n",
          label,
          metrics->avg_waiting,
          metrics->avg_turnaround,
          metrics->avg_response,
          metrics->throughput,
          metrics->cpu_utilization,
          metrics->deadline_misses,
          metrics->migrations);
}

void free_result(SimulationResult *result)
{
  free(result->timeline.slots);
  result->timeline.slots = NULL;
  result->timeline.tick_count = 0;
  result->timeline.cpu_count = 0;
}
