# G20 Operating Systems Lab Projects

Operating Systems Lab Project Submission for `NSCS210/CSC211`, Department of CSE.

## Team Members

- 24JE0701 - Sreenandan Shashidharan
- 24JE0702 - Sukrat Singh Kushwaha
- 24JE0703 - Sura Manohar Sagar
- 24JE0704 - Suraj Kumar Prajapati
- 24JE0705 - Suryansh Kulshreshtha
- 24JE0706 - T Mokshitha

## Repository Structure

- `G20_Project1_xv6CustomizeSystemCalls/`
- `G20_Project2_2/`
- `DOCUMENTATION/`

## Project 1 Summary

`G20_Project1_xv6CustomizeSystemCalls` extends `xv6-riscv` with six syscall-oriented feature groups:

- signals and alarms: `sigalarm`, `sigreturn`, `sigsend`
- thread-style execution and joining: `clone`, `join`
- mailbox-based inter-process communication: `msgsend`, `msgrecv`
- kernel-backed synchronization primitives: `sem_init`, `sem_wait`, `sem_post`
- process creation and priority metadata: `forkprio`, `setpriority`, `getpriority`
- process accounting and inspection: `waitx`, `getpinfo`

The project includes dedicated user programs for demonstrating each feature group and a report with screenshots in the project documentation folder.

## Project 1 Screenshots

### Signal and Alarm Demo

![Project 1 Signal Demo](G20_Project1_xv6CustomizeSystemCalls/screenshots/signaldemo.png)

### Clone and Join Demo

![Project 1 Thread Demo](G20_Project1_xv6CustomizeSystemCalls/screenshots/threadtest.png)

### Mailbox IPC Demo

![Project 1 IPC Demo](G20_Project1_xv6CustomizeSystemCalls/screenshots/ipc_demo.png)

### Semaphore Demo

![Project 1 Semaphore Demo](G20_Project1_xv6CustomizeSystemCalls/screenshots/semdemo.png)

### Priority Demo

![Project 1 Priority Demo](G20_Project1_xv6CustomizeSystemCalls/screenshots/prioritydemo.png)

### waitx Demo

![Project 1 waitx Demo](G20_Project1_xv6CustomizeSystemCalls/screenshots/test_waitx.png)

### getpinfo Demo

![Project 1 getpinfo Demo](G20_Project1_xv6CustomizeSystemCalls/screenshots/getpinfo_test.png)

## Project 2 Summary

`G20_Project2_2` implements an advanced scheduling simulator for multiprocessor systems with:

- Multilevel Feedback Queue (`mlfq`)
- Lottery Scheduling (`lottery`)
- Earliest Deadline First (`edf`)
- a custom affinity-aware multiprocessor scheduler (`custom`)

Additional features include workload-based evaluation, ASCII schedule visualization, CSV export, migration and deadline metrics, and graph generation for comparison.

## Project 2 Screenshots

### Sample Mixed Workload Terminal Output

![Project 2 Sample Terminal](G20_Project2_2/docs/sample_terminal.png)

### Sample Mixed Workload CSV View

![Project 2 Sample CSV](G20_Project2_2/docs/sample_csv.png)

### Sample Mixed Workload Comparison Graph Across All Algorithms

![Project 2 Sample Graph](G20_Project2_2/docs/sample_comparison.png)

### Affinity Workload Terminal Output

![Project 2 Affinity Terminal](G20_Project2_2/docs/affinity_terminal.png)

### Affinity Workload CSV View

![Project 2 Affinity CSV](G20_Project2_2/docs/affinity_csv.png)

### Affinity Workload Comparison Graph Across All Algorithms

![Project 2 Affinity Graph](G20_Project2_2/docs/affinity_comparison.png)

## Documentation

Each project folder contains:

- source code
- build files
- project-specific README
- documentation report
- screenshots and result artifacts required for evaluation
