# G20 Operating Systems Lab Projects

Operating Systems Lab Project Submission for `NSCS210/CSC211`, Department of CSE.

## Team Members

- Sreenandan Shashidharan
- Sukrat Singh Kushwaha
- Sura Manohar Sagar
- Suraj Kumar Prajapati
- Suryansh Kulshreshtha
- T Mokshitha

## Repository Structure

- `G20_Project1_xv6CustomizeSystemCalls/`
- `G20_Project2_2/`

## Project 1 Summary

`G20_Project1_xv6CustomizeSystemCalls` extends `xv6-riscv` with six syscall-oriented feature groups:

- signals and alarms: `sigalarm`, `sigreturn`, `sigsend`
- thread-style execution and joining: `clone`, `join`
- mailbox-based inter-process communication: `msgsend`, `msgrecv`
- kernel-backed synchronization primitives: `sem_init`, `sem_wait`, `sem_post`
- process creation and priority metadata: `forkprio`, `setpriority`, `getpriority`
- process accounting and inspection: `waitx`, `getpinfo`

The project includes dedicated user programs for demonstrating each feature group and a report with screenshots in the project documentation folder.

## Project 2 Summary

`G20_Project2_2` implements an advanced scheduling simulator for multiprocessor systems with:

- Multilevel Feedback Queue (`mlfq`)
- Lottery Scheduling (`lottery`)
- Earliest Deadline First (`edf`)
- a custom affinity-aware multiprocessor scheduler (`custom`)

Additional features include workload-based evaluation, ASCII schedule visualization, CSV export, migration and deadline metrics, and graph generation for comparison.

## Documentation

Each project folder contains:

- source code
- build files
- project-specific README
- documentation report
- screenshots and result artifacts required for evaluation
