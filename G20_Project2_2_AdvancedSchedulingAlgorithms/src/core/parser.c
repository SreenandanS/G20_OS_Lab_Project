#include "sim.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *trim(char *value)
{
  char *end;

  while (*value != '\0' && isspace((unsigned char)*value)) {
    value++;
  }
  if (*value == '\0') {
    return value;
  }

  end = value + strlen(value) - 1;
  while (end > value && isspace((unsigned char)*end)) {
    *end = '\0';
    end--;
  }
  return value;
}

static int parse_bursts(const char *text, ProcessSpec *proc)
{
  char buffer[512];
  char *token;
  char *saveptr = NULL;
  int burst_count = 0;
  BurstType expected = BURST_CPU;

  if (strlen(text) >= sizeof(buffer)) {
    return -1;
  }

  snprintf(buffer, sizeof(buffer), "%s", text);
  token = strtok_r(buffer, ":", &saveptr);
  while (token != NULL) {
    long duration;
    char type_char;

    token = trim(token);
    if (strlen(token) < 2) {
      return -1;
    }

    type_char = (char)toupper((unsigned char)token[0]);
    duration = strtol(token + 1, NULL, 10);
    if (duration <= 0 || burst_count >= MAX_BURSTS) {
      return -1;
    }

    if ((expected == BURST_CPU && type_char != 'C') ||
        (expected == BURST_IO && type_char != 'I')) {
      return -1;
    }

    proc->bursts[burst_count].type =
        (type_char == 'C') ? BURST_CPU : BURST_IO;
    proc->bursts[burst_count].duration = (int)duration;
    burst_count++;
    expected = (expected == BURST_CPU) ? BURST_IO : BURST_CPU;
    token = strtok_r(NULL, ":", &saveptr);
  }

  proc->burst_count = burst_count;
  return burst_count > 0 ? 0 : -1;
}

int parse_workload(const char *path, Workload *workload)
{
  FILE *file;
  char line[1024];
  int line_number = 0;

  memset(workload, 0, sizeof(*workload));
  file = fopen(path, "r");
  if (file == NULL) {
    perror(path);
    return -1;
  }

  while (fgets(line, sizeof(line), file) != NULL) {
    ProcessSpec *proc;
    char *fields[7];
    char *token;
    char *saveptr = NULL;
    int field_count = 0;
    char local[1024];

    line_number++;
    if (line_number == 1 && strncmp(line, "pid,", 4) == 0) {
      continue;
    }

    if (workload->process_count >= MAX_PROCESSES) {
      fclose(file);
      return -1;
    }

    snprintf(local, sizeof(local), "%s", line);
    token = strtok_r(local, ",\n", &saveptr);
    while (token != NULL && field_count < 7) {
      fields[field_count++] = trim(token);
      token = strtok_r(NULL, ",\n", &saveptr);
    }

    if (field_count != 7) {
      fclose(file);
      return -1;
    }

    proc = &workload->processes[workload->process_count];
    memset(proc, 0, sizeof(*proc));
    proc->pid = (int)strtol(fields[0], NULL, 10);
    proc->arrival = (int)strtol(fields[1], NULL, 10);
    proc->base_priority = (int)strtol(fields[3], NULL, 10);
    proc->tickets = (int)strtol(fields[4], NULL, 10);
    proc->deadline = (int)strtol(fields[5], NULL, 10);
    proc->affinity = (int)strtol(fields[6], NULL, 10);

    if (proc->pid <= 0 || proc->arrival < 0 || parse_bursts(fields[2], proc) < 0) {
      fclose(file);
      return -1;
    }

    if (proc->tickets <= 0) {
      proc->tickets = 1;
    }

    workload->process_count++;
  }

  fclose(file);
  return workload->process_count > 0 ? 0 : -1;
}

int parse_algorithm_kind(const char *value, AlgorithmKind *kind)
{
  if (strcmp(value, "mlfq") == 0) {
    *kind = ALGO_MLFQ;
    return 0;
  }
  if (strcmp(value, "lottery") == 0) {
    *kind = ALGO_LOTTERY;
    return 0;
  }
  if (strcmp(value, "edf") == 0) {
    *kind = ALGO_EDF;
    return 0;
  }
  if (strcmp(value, "custom") == 0) {
    *kind = ALGO_CUSTOM;
    return 0;
  }
  if (strcmp(value, "all") == 0) {
    *kind = ALGO_ALL;
    return 0;
  }
  return -1;
}

const char *algorithm_kind_name(AlgorithmKind kind)
{
  switch (kind) {
  case ALGO_MLFQ:
    return "mlfq";
  case ALGO_LOTTERY:
    return "lottery";
  case ALGO_EDF:
    return "edf";
  case ALGO_CUSTOM:
    return "custom";
  case ALGO_ALL:
    return "all";
  }
  return "unknown";
}
