ulimit -c 0
exec /workspaces/binutils-gdb/gdb/testsuite/../../gdb/gdb "$@"
