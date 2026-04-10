# Project 1 Validation Notes

## Current State

- xv6-riscv source has been vendored directly into `G20_Project1_xv6CustomizeSystemCalls/`
- syscall touchpoints for Sreenandan's signals/alarms feature are in place
- runtime verification is pending because the current Mac lacks the official xv6-riscv RISC-V newlib toolchain and QEMU environment

## Later Validation Checklist

```bash
sudo apt update
sudo apt install -y build-essential bc gdb-multiarch qemu-system-misc gcc-riscv64-unknown-elf binutils-riscv64-unknown-elf git make

cd G20_Project1_xv6CustomizeSystemCalls
make qemu
```

## What To Verify

- `sigalarm` periodically diverts user execution to the registered handler
- `sigreturn` restores the saved RISC-V user trapframe
- `sigsend` queues a software signal for another process and the receiver handles it through the installed handler
- `signaldemo` prints both alarm and user-sent signal events
