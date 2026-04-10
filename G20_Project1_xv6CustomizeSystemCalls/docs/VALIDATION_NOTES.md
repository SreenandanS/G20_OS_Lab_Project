# Project 1 Validation Notes

## Current State

- xv6 x86 source has been vendored into `xv6-public/`
- syscall touchpoints for Sreenandan's signals/alarms feature are in place
- runtime verification is pending because the current Mac lacks the required x86 build and QEMU environment

## Later Validation Checklist

```bash
sudo apt update
sudo apt install -y build-essential gdb gcc-multilib qemu-system-x86 git make

cd G20_Project1_xv6CustomizeSystemCalls/xv6-public
make qemu-nox
```

## What To Verify

- `sigalarm` periodically diverts user execution to the registered handler
- `sigreturn` restores the saved x86 user trapframe
- `sigsend` queues a software signal for another process and the receiver handles it through the installed handler
- `signaldemo` prints both alarm and user-sent signal events
