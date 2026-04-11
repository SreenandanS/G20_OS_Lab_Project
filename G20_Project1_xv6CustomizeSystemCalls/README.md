# G20 Project 1: xv6 Customize System Calls

This project extends the RISC-V version of `mit-pdos/xv6-riscv` with six feature groups distributed across the team.

## Team Members

- Sreenandan Shashidharan
- Sukrat Singh Kushwaha
- Sura Manohar Sagar
- Suraj Kumar Prajapati
- Suryansh Kulshreshtha
- T Mokshitha

## Scope

- Base kernel: xv6-riscv from `mit-pdos/xv6-riscv`
- Focus: process control, signaling, synchronization, IPC, priority, and observability
- Current implementation wave: scaffolded RISC-V tree plus Sreenandan's signal/alarm feature area

## Important Note

The current development machine is macOS arm64 and does not have the official xv6-riscv toolchain installed. The upstream repository expects a RISC-V newlib toolchain such as `riscv64-unknown-elf-gcc` together with QEMU `riscv64-softmmu`. The code is organized now, but final build/run verification must happen later on Ubuntu or lab Linux with:

- `build-essential`
- `bc`
- `gdb-multiarch`
- `qemu-system-misc`
- `gcc-riscv64-unknown-elf`
- `binutils-riscv64-unknown-elf`

Later validation command:

```bash
make qemu
```

## Sreenandan's Feature

Sreenandan owns:

- `sigalarm(int ticks, handler)`
- `sigreturn()`
- `sigsend(int pid, int signum)`

These additions are designed for xv6-riscv trapframe-based delivery and are implemented inside the real kernel syscall and trap paths.

## Team Split

- Sreenandan Shashidharan: `sigalarm`, `sigreturn`, `sigsend`
- Sukrat Singh Kushwaha: `clone`, `join`
- Sura Manohar Sagar: `msgsend`, `msgrecv`
- Suraj Kumar Prajapati: `sem_init`, `sem_wait`, `sem_post`
- Suryansh Kulshreshtha: `forkprio`, `setpriority`, `getpriority`
- T Mokshitha: `waitx`, `getpinfo`

## Suryansh's Feature

Suryansh owns the priority metadata syscall set:

- `forkprio(int priority)`: fork a child and set child priority metadata at creation.
- `setpriority(int pid, int priority)`: update an existing process's priority metadata.
- `getpriority(int pid)`: fetch process priority metadata.

### Semantics

- Priority metadata range is `0..100`.
- Default priority for new processes is `50`.
- `fork()` keeps parent priority in child.
- `forkprio()` overrides child priority with caller-supplied value.
- `setpriority()` and `getpriority()` return `-1` for invalid `pid` or invalid priority input.

### Files Updated For This Feature

- `kernel/proc.h` (process metadata field)
- `kernel/proc.c` (priority logic, `kforkprio`, `setpriority`, `getpriority`)
- `kernel/defs.h` (kernel declarations)
- `kernel/sysproc.c` (syscall handlers)
- `kernel/syscall.h` and `kernel/syscall.c` (syscall number and dispatch wiring)
- `user/user.h` and `user/usys.pl` (userspace ABI)
- `user/prioritydemo.c` (userspace demo)

### Demo Program

`prioritydemo` shows:

1. Parent reading its own priority.
2. Child created with `forkprio(30)`.
3. Child mutating metadata via `setpriority(..., 80)`.
4. Parent reading child metadata via `getpriority(child_pid)` before wait.

Run inside xv6 shell:

```bash
prioritydemo
```
