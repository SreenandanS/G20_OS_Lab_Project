# G20 Project 1: xv6 Customize System Calls

This project extends the RISC-V version of `mit-pdos/xv6-riscv` with six feature groups distributed across the team.

## Scope

- Base kernel: xv6-riscv from `mit-pdos/xv6-riscv`
- Focus: process control, signaling, synchronization, IPC, priority, and observability
- Current implementation wave: scaffolded RISC-V tree plus Sreenandan's signal/alarm feature area

## Important Note

The current development machine is macOS arm64 and does not have the xv6-riscv toolchain installed. The code is organized now, but final build/run verification must happen later on Ubuntu or lab Linux with:

- `build-essential`
- `bc`
- `gdb-multiarch`
- `qemu-system-misc`
- `gcc-riscv64-linux-gnu`
- `binutils-riscv64-linux-gnu`

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
