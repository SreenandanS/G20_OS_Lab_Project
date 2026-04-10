#include "sim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage(const char *argv0)
{
  fprintf(stderr,
          "Usage: %s --algo {mlfq,lottery,edf,custom,all} --cpus N --input <workload.csv> --output-dir <dir> [--seed N] [--max-ticks N]\n",
          argv0);
}

static int parse_args(int argc, char **argv, SimulationConfig *config)
{
  int i;

  memset(config, 0, sizeof(*config));
  config->cpu_count = 4;
  config->seed = 42;
  config->max_ticks = 512;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--algo") == 0 && i + 1 < argc) {
      if (parse_algorithm_kind(argv[++i], &config->algorithm) < 0) {
        return -1;
      }
      config->run_all = (config->algorithm == ALGO_ALL);
    } else if (strcmp(argv[i], "--cpus") == 0 && i + 1 < argc) {
      config->cpu_count = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
      snprintf(config->input_path, sizeof(config->input_path), "%s", argv[++i]);
    } else if (strcmp(argv[i], "--output-dir") == 0 && i + 1 < argc) {
      snprintf(config->output_dir, sizeof(config->output_dir), "%s", argv[++i]);
    } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
      config->seed = (unsigned)strtoul(argv[++i], NULL, 10);
    } else if (strcmp(argv[i], "--max-ticks") == 0 && i + 1 < argc) {
      config->max_ticks = atoi(argv[++i]);
    } else {
      return -1;
    }
  }

  if (config->cpu_count <= 0 || config->cpu_count > MAX_CPUS ||
      config->input_path[0] == '\0' || config->output_dir[0] == '\0') {
    return -1;
  }

  return 0;
}

int main(int argc, char **argv)
{
  SimulationConfig config;
  Workload workload;
  SimulationResult results[4];
  AlgorithmKind algorithms[] = {ALGO_MLFQ, ALGO_LOTTERY, ALGO_EDF, ALGO_CUSTOM};
  int run_count = 0;
  int i;

  if (parse_args(argc, argv, &config) < 0) {
    print_usage(argv[0]);
    return 1;
  }

  if (parse_workload(config.input_path, &workload) < 0) {
    fprintf(stderr, "Failed to parse workload: %s\n", config.input_path);
    return 1;
  }

  if (ensure_output_dir(config.output_dir) < 0) {
    fprintf(stderr, "Failed to create output directory: %s\n", config.output_dir);
    return 1;
  }

  if (config.run_all) {
    for (i = 0; i < 4; i++) {
      if (run_simulation(&workload, &config, algorithms[i], &results[run_count]) < 0) {
        fprintf(stderr, "Simulation failed for %s\n", algorithm_kind_name(algorithms[i]));
        return 1;
      }
      if (write_result_files(config.output_dir, config.input_path, &results[run_count]) < 0) {
        fprintf(stderr, "Failed to write result files for %s\n",
                results[run_count].metrics.algorithm_name);
        return 1;
      }
      print_metrics(stdout, results[run_count].metrics.algorithm_name,
                    &results[run_count].metrics);
      run_count++;
    }
    if (write_comparison_csv(config.output_dir, results, run_count) < 0) {
      fprintf(stderr, "Failed to write comparison CSV\n");
      return 1;
    }
  } else {
    if (run_simulation(&workload, &config, config.algorithm, &results[0]) < 0) {
      fprintf(stderr, "Simulation failed for %s\n", algorithm_kind_name(config.algorithm));
      return 1;
    }
    if (write_result_files(config.output_dir, config.input_path, &results[0]) < 0 ||
        write_comparison_csv(config.output_dir, results, 1) < 0) {
      fprintf(stderr, "Failed to write output files\n");
      return 1;
    }
    print_metrics(stdout, results[0].metrics.algorithm_name, &results[0].metrics);
    run_count = 1;
  }

  for (i = 0; i < run_count; i++) {
    free_result(&results[i]);
  }

  return 0;
}
