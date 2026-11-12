#!/bin/bash
# Wrapper to run GDB outside the snap sandbox
# The snap confinement of VSCode interferes with GDB's ptrace
unset LD_LIBRARY_PATH
exec /usr/bin/gdb "$@"
