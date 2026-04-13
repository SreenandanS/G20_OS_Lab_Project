# G20 Documentation

This folder collects the final submission-facing documentation for both Operating Systems Lab projects in one place.

## Contents

- `Project1_Report.md`
- `Project2_Report.md`
- `screenshots/project1/`
- `screenshots/project2/`

## Contribution

- `24JE0701 - Sreenandan Shashidharan`: implemented xv6-riscv signal and alarm support with `sigalarm`, `sigreturn`, and `sigsend`, and built the shared Project 2 simulator foundation plus the custom affinity-aware multiprocessor scheduler.
- `24JE0702 - Sukrat Singh Kushwaha`: implemented thread-style worker creation and synchronization with `clone` and `join`, and contributed the MLFQ scheduling algorithm for Project 2.
- `24JE0703 - Sura Manohar Sagar`: implemented mailbox-based IPC with `msgsend` and `msgrecv`, and contributed the Lottery Scheduler implementation for Project 2.
- `24JE0704 - Suraj Kumar Prajapati`: implemented kernel semaphore support with `sem_init`, `sem_wait`, and `sem_post`, and contributed the EDF scheduler for Project 2.
- `24JE0705 - Suryansh Kulshreshtha`: implemented process priority metadata support with `forkprio`, `setpriority`, and `getpriority`, and contributed workload design plus metrics validation for Project 2.
- `24JE0706 - T Mokshitha`: implemented process-accounting support with `waitx` and `getpinfo`, and contributed plotting, documentation, result presentation, and screenshot organization for Project 2.

## Project 1

- Report: `Project1_Report.md`
- Screenshot folder: `screenshots/project1/`

Included execution evidence:

- `signaldemo`
- `threadtest`
- `ipc_demo`
- `semdemo`
- `prioritydemo`
- `test_waitx`
- `getpinfo_test`

## Project 2

- Report: `Project2_Report.md`
- Screenshot folder: `screenshots/project2/`

Included result evidence:

- sample mixed workload terminal output
- sample mixed workload comparison CSV screenshot
- sample mixed workload comparison graph
- affinity workload terminal output
- affinity workload comparison CSV screenshot
- affinity workload comparison graph
