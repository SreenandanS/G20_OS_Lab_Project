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

## Sukrat's Feature: Lightweight Threads via `clone` / `join`

Sukrat owns the `clone` and `join` system calls, which implement POSIX-style lightweight threading for xv6-riscv.

### API

```c
// Spawn a thread that runs fn(arg) on the given stack.
// Returns the new thread's pid (tid) to the caller; never returns 0.
int clone(void (*fn)(void *), void *stack, void *arg);

// Block until the thread with the given tid exits.
// Returns the tid on success, -1 on error.
int join(int tid);
```

### Design

| Aspect | Detail |
|--------|--------|
| **Shared address space** | `clone()` assigns the **parent's existing `pagetable`** to the new `proc` entry — no copy-on-write, same physical pages |
| **Separate kernel control** | Every thread gets its own `struct proc` (kernel stack, trapframe, scheduling state) so the kernel can schedule and block threads independently |
| **Stack** | Caller allocates a page; passes the **top** of the page (RISC-V stacks grow downward). The thread's `sp` register is set to this value in the trapframe |
| **Entry point** | `trapframe->epc` is patched to `fn`; `trapframe->a0` is set to `arg` |
| **Cleanup** | `join()` zeroes the `proc` slot and frees only the per-thread **trapframe page**; the shared pagetable is left intact (parent owns it) |
| **Kernel files changed** | `proc.h`, `proc.c`, `sysproc.c`, `defs.h`, `syscall.h`, `syscall.c` |
| **Syscall numbers** | `SYS_clone = 25`, `SYS_join = 26` |

### Demo program

```
$ threadtest
threadtest: parent pid=3
threadtest: started thread 0 with tid=4
threadtest: started thread 1 with tid=5
thread[0]: started (tid=4), writing result
thread[0]: results[0] = 100, exiting
thread[1]: started (tid=5), writing result
thread[1]: results[1] = 200, exiting
threadtest: joined tid0=4 (ret=4) tid1=5 (ret=5)
threadtest: results[0]=100 results[1]=200
threadtest: PASS - shared memory updated by threads
```

### How to run (on Ubuntu/lab Linux)

```bash
cd G20_Project1_xv6CustomizeSystemCalls
make qemu
# Inside xv6 shell:
$ threadtest
```
