# G20 Team Execution Guide

This guide is the shared execution contract for the whole group. Follow it strictly so the repo stays clean and each person's contribution is visible in code, commits, and viva.

## Common Rules

### Repo Structure

- Keep all xv6-riscv work inside `G20_Project1_xv6CustomizeSystemCalls/`.
- Keep all scheduling simulator work inside `G20_Project2_2_AdvancedSchedulingAlgorithms/`.
- Do not create extra top-level folders.
- Add documentation next to the project it belongs to.

### Code Standards

- Keep code neat, clean, minimal, and directly tied to the requirement.
- Prefer simple arrays and small helper functions over overly clever abstractions.
- Keep advanced logic only where it clearly improves marks, comparison quality, or clarity.
- Avoid dead code, commented-out blocks, and duplicate helpers.
- Use comments only when the kernel or scheduler logic is non-obvious.
- Preserve xv6-riscv naming and conventions in Project 1. Do not introduce x86-specific files or APIs.
- Keep Project 2 interfaces stable once merged: CLI flags, workload schema, metrics CSV names, and output layout should not change without discussion.

### PR Rules

- Work only on your assigned branch.
- Do not force-push to `main`.
- Keep PRs text-only. Do not add screenshots to pull requests.
- Every PR description must include scope, exact files touched, test commands run, and what behavior changed.
- Each person updates documentation for their own contribution before requesting merge.

### Merge Order

1. `sreenandan/signals-custom-foundation`
2. Project 1 syscall branches
3. Project 2 algorithm branches
4. metrics/docs/graphs polish branch

### Validation Notes

- The current Mac does not have `qemu-system-riscv64` or the upstream xv6-riscv `riscv64-unknown-elf` toolchain installed.
- Project 1 code can be written now, but runtime validation will happen later on Ubuntu VM or lab Linux.
- Recommended Project 1 Linux packages:
  - `build-essential`
  - `bc`
  - `gdb-multiarch`
  - `qemu-system-misc`
  - `gcc-riscv64-unknown-elf`
  - `binutils-riscv64-unknown-elf`
- This matches the official `mit-pdos/xv6-riscv` expectation of a RISC-V newlib toolchain and QEMU for `riscv64-softmmu`.
- Project 2 can still be built locally on macOS right away.

## Branch Plan

- `sreenandan/signals-custom-foundation`
- `sukrat/threads-mlfq`
- `sura/ipc-lottery`
- `suraj/semaphores-edf`
- `suryansh/priority-metrics`
- `mokshitha/docs-graphs`

## Sreenandan Shashidharan

### Ownership

- Project 1: `sigalarm`, `sigreturn`, `sigsend`
- Project 2: `custom` scheduler plus the simulator foundation

### Project 1 Files

- `G20_Project1_xv6CustomizeSystemCalls/kernel/proc.h`
- `G20_Project1_xv6CustomizeSystemCalls/kernel/proc.c`
- `G20_Project1_xv6CustomizeSystemCalls/kernel/trap.c`
- `G20_Project1_xv6CustomizeSystemCalls/kernel/defs.h`
- `G20_Project1_xv6CustomizeSystemCalls/kernel/syscall.h`
- `G20_Project1_xv6CustomizeSystemCalls/kernel/syscall.c`
- `G20_Project1_xv6CustomizeSystemCalls/kernel/sysproc.c`
- `G20_Project1_xv6CustomizeSystemCalls/user/user.h`
- `G20_Project1_xv6CustomizeSystemCalls/user/usys.pl`
- `G20_Project1_xv6CustomizeSystemCalls/user/signaldemo.c`
- `G20_Project1_xv6CustomizeSystemCalls/Makefile`
- Project 1 README/docs updates for the signal feature

### Project 2 Files

- `G20_Project2_2_AdvancedSchedulingAlgorithms/include/*.h`
- `G20_Project2_2_AdvancedSchedulingAlgorithms/src/core/*.c`
- `G20_Project2_2_AdvancedSchedulingAlgorithms/src/schedulers/custom.c`
- `G20_Project2_2_AdvancedSchedulingAlgorithms/src/main.c`
- `G20_Project2_2_AdvancedSchedulingAlgorithms/Makefile`
- Project 2 README/docs for CLI, workload format, and output artifacts

### Commands

```bash
git checkout -b sreenandan/signals-custom-foundation

cd G20_Project2_2_AdvancedSchedulingAlgorithms
make
./bin/scheduler_sim --algo custom --cpus 4 --input data/workloads/sample_mixed.csv --output-dir out/sample_run

# Later on Ubuntu or lab Linux
cd ../G20_Project1_xv6CustomizeSystemCalls
make qemu
```

### Definition Of Done

- Project 1 RISC-V signal code is wired into the real syscall, process, and trap paths.
- `sigalarm` installs the user handler and periodic alarm interval.
- `sigreturn` restores the saved RISC-V trapframe cleanly.
- `sigsend` queues a software signal for another process.
- Project 2 foundation builds on macOS.
- `custom` runs on the sample workload and writes summary, timeline, and CSV output.

### Viva Points

- Explain how xv6-riscv returns to user mode using `usertrap()` and the saved trapframe.
- Explain why `sigreturn` must restore the earlier trapframe rather than only clearing a flag.
- Explain how `custom` uses affinity, aging, and work stealing together.
- Explain what parts are intentionally postponed until the RISC-V environment is ready.

### Integration Guardrails

- Do not edit other members' Project 1 syscalls unless integration requires a shared prototype update.
- Keep the Project 2 CLI and CSV output stable for the rest of the team.

## Sukrat Singh Kushwaha

### Ownership

- Project 1: `clone`, `join`
- Project 2: `mlfq`

### Expected Files

- Project 1:
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/proc.c`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/proc.h`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/sysproc.c`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/defs.h`
  - `G20_Project1_xv6CustomizeSystemCalls/user/user.h`
  - `G20_Project1_xv6CustomizeSystemCalls/user/usys.pl`
  - one user demo program
- Project 2:
  - `G20_Project2_2_AdvancedSchedulingAlgorithms/src/schedulers/mlfq.c`

### Commands

```bash
git checkout -b sukrat/threads-mlfq
cd G20_Project2_2_AdvancedSchedulingAlgorithms
make
./bin/scheduler_sim --algo mlfq --cpus 4 --input data/workloads/sample_mixed.csv --output-dir out/mlfq_run
```

### Definition Of Done

- `clone` and `join` work through xv6-riscv process and trap control paths.
- `mlfq` implements 3 queues with quantums `2/4/8`.
- Periodic priority boost is visible in code and output behavior.

### Viva Points

- Explain how lightweight threads share address space but still need separate kernel control structures.
- Explain why MLFQ helps interactive jobs without fully starving long CPU-bound jobs.

### Integration Guardrails

- Do not rename shared scheduler interfaces.
- Keep MLFQ-specific constants in the scheduler module, not scattered across core files.

## Sura Manohar Sagar

### Ownership

- Project 1: `msgsend`, `msgrecv`
- Project 2: `lottery`

### Expected Files

- Project 1:
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/proc.c`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/proc.h`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/sysproc.c`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/defs.h`
  - `G20_Project1_xv6CustomizeSystemCalls/user/user.h`
  - `G20_Project1_xv6CustomizeSystemCalls/user/usys.pl`
  - one user demo program
- Project 2:
  - `G20_Project2_2_AdvancedSchedulingAlgorithms/src/schedulers/lottery.c`

### Commands

```bash
git checkout -b sura/ipc-lottery
cd G20_Project2_2_AdvancedSchedulingAlgorithms
make
./bin/scheduler_sim --algo lottery --cpus 4 --input data/workloads/sample_mixed.csv --output-dir out/lottery_run
```

### Definition Of Done

- mailbox message delivery works in xv6-riscv with clear ownership semantics
- lottery scheduling is deterministic under the fixed seed and uses ticket counts from the workload

### Viva Points

- Explain how IPC introduces process coordination without shared memory.
- Explain how randomness is controlled so lottery results remain reproducible.

### Integration Guardrails

- Do not replace the shared CSV schema.
- Keep IPC buffer sizes and mailbox rules documented in your own doc section.

## Suraj Kumar Prajapati

### Ownership

- Project 1: `sem_init`, `sem_wait`, `sem_post`
- Project 2: `edf`

### Expected Files

- Project 1:
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/proc.c`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/proc.h`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/sysproc.c`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/defs.h`
  - `G20_Project1_xv6CustomizeSystemCalls/user/user.h`
  - `G20_Project1_xv6CustomizeSystemCalls/user/usys.pl`
  - one user demo program
- Project 2:
  - `G20_Project2_2_AdvancedSchedulingAlgorithms/src/schedulers/edf.c`

### Commands

```bash
git checkout -b suraj/semaphores-edf
cd G20_Project2_2_AdvancedSchedulingAlgorithms
make
./bin/scheduler_sim --algo edf --cpus 4 --input data/workloads/sample_mixed.csv --output-dir out/edf_run
```

### Definition Of Done

- semaphore operations are safe enough for the xv6-riscv teaching kernel
- EDF chooses the earliest deadline among ready tasks and reports deadline misses

### Viva Points

- Explain why semaphores need atomic update and wakeup behavior.
- Explain why EDF is effective for real-time oriented workloads.

### Integration Guardrails

- Do not move shared metrics logic into the EDF module.
- Keep semaphore-specific constants and data structures localized.

## Suryansh Kulshreshtha

### Ownership

- Project 1: `forkprio`, `setpriority`, `getpriority`
- Project 2: workloads and metrics validation

### Expected Files

- Project 1:
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/proc.c`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/proc.h`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/sysproc.c`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/defs.h`
  - `G20_Project1_xv6CustomizeSystemCalls/user/user.h`
  - `G20_Project1_xv6CustomizeSystemCalls/user/usys.pl`
  - one user demo program
- Project 2:
  - `G20_Project2_2_AdvancedSchedulingAlgorithms/data/workloads/*.csv`
  - `G20_Project2_2_AdvancedSchedulingAlgorithms/docs/metrics_validation.md`

### Commands

```bash
git checkout -b suryansh/priority-metrics
cd G20_Project2_2_AdvancedSchedulingAlgorithms
make
./bin/scheduler_sim --algo all --cpus 4 --input data/workloads/sample_mixed.csv --output-dir out/all_run
```

### Definition Of Done

- priority syscalls change process metadata cleanly in xv6-riscv
- at least four workloads exist and each one highlights a different scheduler behavior
- documented metrics match the generated CSV output

### Viva Points

- Explain why scheduler experiments need carefully designed workloads.
- Explain the difference between priority input and measured performance.

### Integration Guardrails

- Do not edit scheduler algorithm logic unless a core interface change is approved first.
- Keep workload files human-readable and commented in docs.

## T Mokshitha

### Ownership

- Project 1: `waitx`, `getpinfo`
- Project 2: plotting, documentation, screenshots

### Expected Files

- Project 1:
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/proc.c`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/proc.h`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/sysproc.c`
  - `G20_Project1_xv6CustomizeSystemCalls/kernel/defs.h`
  - `G20_Project1_xv6CustomizeSystemCalls/user/user.h`
  - `G20_Project1_xv6CustomizeSystemCalls/user/usys.pl`
  - one user demo program
- Project 2:
  - `G20_Project2_2_AdvancedSchedulingAlgorithms/scripts/plot_metrics.py`
  - `G20_Project2_2_AdvancedSchedulingAlgorithms/docs/`
  - final screenshots and result summaries

### Commands

```bash
git checkout -b mokshitha/docs-graphs
cd G20_Project2_2_AdvancedSchedulingAlgorithms
make
./bin/scheduler_sim --algo all --cpus 4 --input data/workloads/sample_mixed.csv --output-dir out/final_run
python3 scripts/plot_metrics.py out/final_run/comparison.csv
```

### Definition Of Done

- wait/runtime reporting works in xv6-riscv once the environment is ready
- graph generation consumes the generated CSV files without manual edits
- documentation includes execution flow, observations, and final screenshot placeholders

### Viva Points

- Explain how measured metrics connect to algorithm behavior.
- Explain how visual outputs help compare scheduling policies quickly.

### Integration Guardrails

- Do not rewrite scheduler output formats after others have started using them.
- Keep docs high-signal and tied to actual code behavior.
