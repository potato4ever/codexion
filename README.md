*This project has been created as part of the 42 curriculum by <login1>.*

# Codexion

## Description

Codexion is a POSIX-thread simulation in which coders share a circular set of
USB dongles. A coder needs both adjacent dongles to compile, then debugs and
refactors before trying again. The program stops when every coder completes the
requested number of compilations or when the monitor detects a coder burnout.

Requests are arbitrated with either FIFO or EDF (Earliest Deadline First). EDF
uses the next burnout deadline and a deterministic arrival-order/coder-ID tie
break.

## Instructions

Build the program:

```sh
make
```

Run it:

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown fifo|edf
```

All times are milliseconds. For example:

```sh
./codexion 4 1000 150 100 100 3 25 edf
```

Use `make clean`, `make fclean`, or `make re` to remove build outputs.

## Blocking cases handled

- A coder requests both dongles as one scheduler request; it never holds one
  while waiting for the other. This eliminates hold-and-wait, breaking a
  Coffman deadlock condition.
- Every dongle has a mutex-protected request heap. FIFO uses request arrival
  order; EDF uses the earliest deadline, then arrival order and coder ID.
  A request is granted only when it is the policy-selected request at both of
  its dongles.
- Releases set a cooldown deadline before the dongles can be selected again.
  Timed condition waits wake the scheduler at cooldown expiry without busy
  looping.
- A dedicated monitor waits for the nearest compile-start deadline and prints
  the burnout event immediately after detecting it.
- Log output is serialized, and terminal state is protected so ordinary state
  logs cannot appear after a burnout announcement.

## Thread synchronization mechanisms

- `pthread_mutex_t state_mutex` guards lifecycle state, request scheduling,
  coder deadlines, completion counts, and terminal state.
- Each dongle has its own `pthread_mutex_t`, protecting its busy/cooldown state
  and heap queue. Dongle state is only read or changed while that mutex is held.
- `pthread_cond_t event` lets workers and the monitor sleep until a request,
  release, cooldown deadline, compile-start update, or shutdown changes the
  schedule. Broadcasts prevent missed wakeups.
- `pthread_mutex_t print_mutex` makes each log line atomic. The state mutex is
  held while terminal decisions are printed, preventing output races with the
  monitor.

## Resources

- [POSIX Threads Programming](https://hpc-tutorials.llnl.gov/posix/)
- [pthread_mutex_lock(3p)](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
- [pthread_cond_timedwait(3p)](https://man7.org/linux/man-pages/man3/pthread_cond_timedwait.3p.html)
- [Earliest deadline first scheduling](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)

AI was used to help analyze the supplied assignment, discuss the synchronization
architecture, and review implementation/test ideas. All generated material was
reviewed, compiled, and tested by the project author before inclusion.

## Local test ideas

Exercise invalid arguments, one coder, short burnout windows, non-zero cooldown,
and both policies. For repeated checks, use small valid durations with a finite
compile quota and confirm that execution terminates without mixed log lines.
