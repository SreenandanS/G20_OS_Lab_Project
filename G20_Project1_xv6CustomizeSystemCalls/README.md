# G20 Project 1: xv6 Customize System Calls

## Overview

This project extends the official `mit-pdos/xv6-riscv` teaching operating system with six syscall-oriented feature groups covering process control, communication, synchronization, signals, and observability.

## Team Members

- 24JE0701 - Sreenandan Shashidharan
- 24JE0702 - Sukrat Singh Kushwaha
- 24JE0703 - Sura Manohar Sagar
- 24JE0704 - Suraj Kumar Prajapati
- 24JE0705 - Suryansh Kulshreshtha
- 24JE0706 - T Mokshitha

## Implemented Functionality

### 1. Signals and Alarms

- `sigalarm(int ticks, handler)`
- `sigreturn()`
- `sigsend(int pid, int signum)`

These changes add timer-driven user handlers and user-to-user software signal delivery using the xv6 trapframe return path.

### 2. Thread-Style Execution

- `clone(void (*fn)(void *), void *stack, void *arg)`
- `join(int tid)`

These syscalls create a lightweight worker execution path with explicit joining semantics for controlled synchronization.

### 3. Mailbox IPC

- `msgsend(int pid, char *msg)`
- `msgrecv(char *buf)`

Each process gets a simple kernel mailbox for bounded point-to-point message delivery.

### 4. Semaphores

- `sem_init(int semid, int value)`
- `sem_wait(int semid)`
- `sem_post(int semid)`

The semaphore table is managed inside the kernel and can be used by user programs for synchronization across processes.

### 5. Priority Control

- `forkprio(int priority)`
- `setpriority(int pid, int priority)`
- `getpriority(int pid)`

These syscalls support explicit priority metadata during process creation and later inspection/update.

### 6. Process Accounting

- `waitx(int *rtime, int *wtime)`
- `getpinfo(struct pinfo *info)`

Runtime, ready time, and process-state snapshots are exported for analysis and demonstration.

## Demo Programs

- `signaldemo`
- `threadtest`
- `ipc_demo`
- `semdemo`
- `prioritydemo`
- `test_waitx`
- `getpinfo_test`

## Build and Run

Install a RISC-V xv6 toolchain and QEMU, then run:

```bash
make qemu
```

From the xv6 shell, execute the demo programs directly:

```text
signaldemo
threadtest
ipc_demo
semdemo
prioritydemo
test_waitx
getpinfo_test
```

## Additional Improvements

- integrated timer-driven signal delivery through the xv6 trap return path
- mailbox IPC with blocking receive semantics
- kernel semaphore table for clean synchronization demonstrations
- process accounting data exposed to user space
- multiple dedicated demo programs for clearer evaluation and viva discussion

## Execution Screenshots

### Signal and Alarm Demonstration

![Signal Demo](screenshots/signaldemo.png)

### Clone and Join Demonstration

![Thread Demo](screenshots/threadtest.png)

### Mailbox IPC Demonstration

![IPC Demo](screenshots/ipc_demo.png)

### Semaphore Demonstration

![Semaphore Demo](screenshots/semdemo.png)

### Priority Control Demonstration

![Priority Demo](screenshots/prioritydemo.png)

### Runtime Accounting with `waitx`

![waitx Demo](screenshots/test_waitx.png)

### Process Table Snapshot with `getpinfo`

![getpinfo Demo](screenshots/getpinfo_test.png)

## Documentation

Detailed analysis, demo notes, and execution screenshots are available in:

- `docs/REPORT.md`
- `screenshots/`
