*This project has been created as part of the 42 curriculum by zabelhac.*

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
- A dedicated monitor checks for the nearest compile-start deadline and prints
  the burnout event immediately after detecting it.
- Log output is serialized, and terminal state is protected so ordinary state
  logs cannot appear after a burnout announcement.

## Thread synchronization mechanisms

This simulation manages thread concurrency using POSIX threads (`pthread`) to prevent race conditions and ensure thread-safe communication across worker threads (coders) and the background monitor thread[cite: 1].

### 1. Thread Primitives & Purpose
- **`pthread_mutex_t state_mutex`**: Guards global lifecycle state, including `last_start` timestamps, compilation counts (`compiles`), execution flags (`start_ready`, `stopped`), and terminal state updates[cite: 1].
- **`pthread_mutex_t mutex` (per dongle)**: Protects individual dongle state (`busy`, `cooldown_until`) and its associated priority queue (`t_heap`)[cite: 1].
- **`pthread_mutex_t print_mutex`**: Guarantees atomic logging to `stdout` via `print_event_locked` to prevent overlapping line outputs[cite: 1].
- **`pthread_cond_t event`**: Serves as the global signal condition variable to wake sleeping threads upon state updates (such as simulation start or global shutdown via `pthread_cond_broadcast`)[cite: 1].

### 2. Custom Cooldown Wait & Event Implementation
- **Custom Cooldown Handling**: When a coder thread cannot acquire both adjacent dongles, `wait_until_event` queries the soonest active dongle cooldown timestamp via `find_soonest_cooldown(sim)`[cite: 1]. Threads pause for cooldown completion without busy-looping using targeted sleep intervals[cite: 1].
- **Interruptible Sleep Cycles**: Worker thread state transitions (e.g., debug, refactor, and compilation duration) rely on `sleep_until` and `interruptible_sleep`, which periodically poll `simulation_stopped(sim)` to ensure threads terminate immediately upon global shutdown signals[cite: 1].

### 3. Coder-to-Monitor Thread-Safe Communication
- **Burnout Tracking**: Whenever a coder is granted access to compile, its `last_start` timestamp is locked and updated under `state_mutex`[cite: 1].
- **Monitor Evaluation**: The monitor thread repeatedly locks `state_mutex`, iterates over all coders via `find_soonest` to check for impending burnout deadlines, and determines if a threshold has been breached[cite: 1].
- **Immediate Termination Signal**: If a burnout occurs or all coders fulfill `number_of_compiles_required`, `stop_simulation` acquires `state_mutex`, sets `sim->stopped = 1`, and invokes `pthread_cond_broadcast(&sim->event)` to instantly unblock all waiting threads[cite: 1].

### 4. Race Condition Prevention & Deadlock Avoidance
- **Strict Lock Ordering (`lock_pair`)**: To avoid circular waiting (dining philosophers deadlock), acquiring a pair of adjacent dongles always locks the lower-indexed dongle mutex first[cite: 1].
- **Hold-and-Wait Elimination**: Coders request and check ownership for both adjacent dongles simultaneously inside `try_acquire_self_locked`[cite: 1]. A coder never holds one dongle while blocked waiting for the second[cite: 1].
- **Log Serialization & Post-Burnout Gating**: State logging checks `sim->stopped` prior to calling `print_event_locked`[cite: 1]. The `print_mutex` enforces atomic line emission, ensuring normal coder activity logs never output after a terminal burnout event is reported[cite: 1].

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
