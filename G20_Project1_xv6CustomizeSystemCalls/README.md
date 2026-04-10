# G20 Project 1: xv6 Customize System Calls

This project extends the x86 version of `mit-pdos/xv6-public` with six feature groups distributed across the team.

## Scope

- Base kernel: xv6 x86 from `mit-pdos/xv6-public`
- Focus: process control, signaling, synchronization, IPC, priority, and observability
- Current implementation wave: scaffolded x86 tree plus Sreenandan's signal/alarm feature area

## Important Note

The current development machine is macOS arm64 and does not have the full xv6 x86 toolchain installed. The code is organized now, but final build/run verification must happen later on Ubuntu or lab Linux with:

- `build-essential`
- `gdb`
- `gcc-multilib`
- `qemu-system-x86`

## Sreenandan's Feature

Sreenandan owns:

- `sigalarm(int ticks, handler)`
- `sigreturn()`
- `sigsend(int pid, int signum)`

These additions are designed for xv6 x86 trapframe-based delivery and are implemented inside the real kernel syscall and trap paths.
