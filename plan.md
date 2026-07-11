Master Plan for Codexion
Phase 0 — Understand the Problem

The project has four major concerns that must work together:

Thread management
Resource management (dongles)
Scheduling (FIFO / EDF)
Simulation monitoring

Everything else is built around those.

Phase 1 — Design the Architecture
Core Objects

The project should revolve around a single simulation object.

Simulation
│
├── coders[]
├── dongles[]
├── monitor thread
├── logger
├── global state
└── timing information

Every other structure belongs to it.

Coder Structure

Each coder should contain only information about itself.

id

pthread_t thread

left_dongle
right_dongle

last_compile_start

compile_count

current_state

simulation pointer

Nothing else.

No global variables.

Dongle Structure

Each dongle should be an independent synchronized object.

id

mutex

condition variable

owner

available

cooldown_end_time

priority queue

simulation pointer

The dongle should completely manage its own waiting queue.

Request Structure

Whenever a coder wants a dongle it creates a request.

coder pointer

request timestamp

deadline

heap index

Deadline is only meaningful for EDF.

Priority Queue

Each dongle owns its own heap.

Binary Heap

push()

pop()

peek()

heapify_up()

heapify_down()

The comparator changes according to scheduler.

FIFO:

arrival_time

EDF:

deadline

↓

arrival_time

↓

coder id

This guarantees deterministic behavior.

Logger

One mutex.

One function.

log_action(sim, coder, message)

No printf anywhere else.

Every message passes through this.

Monitor

Separate thread.

Responsibilities:

while simulation running

    check every coder

    detect burnout

    detect completion

    stop simulation

Nothing else.

Phase 2 — Build Utility Modules

These should be written first.

Time Utilities

Need

current_time_ms()

elapsed_time()

precise_sleep(ms)

precise_sleep should never simply call

usleep(ms * 1000)

Instead

while

remaining > 1 ms

    usleep(500)

recheck

This gives accurate timing.

Input Parser

Responsibilities

argument count
integer parsing
overflow
scheduler validation

Reject

negative

zero

non-number

invalid scheduler
Cleanup

One cleanup function.

Destroy

mutexes
condition variables
heap memory
arrays

Everything.

Phase 3 — Heap Implementation

This deserves its own module.

Files

heap.c

heap_utils.c

Functions

heap_init

heap_push

heap_pop

heap_peek

heap_swap

heapify_up

heapify_down

heap_destroy

Comparator should be configurable.

Pseudo

compare(a,b)

↓

fifo

or

edf

No duplicated heap code.

Phase 4 — Dongle Manager

Each dongle exposes only two public operations.

acquire()

release()

Everything else stays internal.

Acquire should

lock mutex

create request

insert heap

while

not first in queue

or cooldown active

or unavailable

    cond_wait

become owner

remove request

unlock

Release

lock

owner=NULL

available=false

cooldown_end = now + cooldown

wake waiting threads

unlock
Cooldown

A released dongle is temporarily unavailable.

Need either

Option A

check cooldown every wakeup

or

Option B

pthread_cond_timedwait()

until cooldown expires

Option B is cleaner and avoids busy waiting.

Phase 5 — Resource Acquisition Strategy

This is the hardest design decision.

The subject requires

fairness
scheduler
cooldown
no starvation
no deadlock

The safest solution is not to lock one dongle and then wait indefinitely for the other.

Instead, treat acquisition as a coordinated protocol.

Recommended approach:

Request both required dongles.
Only proceed when the coder is eligible to receive both according to the scheduler and both dongles are available (including cooldown).
Atomically claim both dongles under a consistent locking order (e.g., always lock the lower-addressed or lower-indexed dongle mutex first, then the other) to avoid mutex deadlocks.
If the second cannot be claimed because conditions changed, release any partial claim, remain in the queues as appropriate, and retry.

This avoids the classic "hold one, wait forever for the other" scenario.

Phase 6 — Coder Routine

Every thread executes

loop

    acquire both dongles

    compile

    release

    debug

    refactor

Compile

last_compile_start = now

print

sleep

compile_count++

Release

release left

release right

Then continue.

Phase 7 — Monitor Thread

Loop

while running

    for every coder

        if

        now-last_compile_start

        >= burnout

            stop simulation

            print burnout

    if everyone done

        stop simulation

    usleep(1000)

One millisecond polling.

Phase 8 — Thread Stop Mechanism

Need one shared variable

simulation_running

Protected by mutex (or made atomic if allowed; here a mutex is simplest).

Every thread checks it frequently.

Monitor sets

false

Broadcasts all relevant condition variables so waiting threads wake up and exit cleanly.

Phase 9 — Initialization

Initialization order

parse

↓

allocate simulation

↓

allocate coders

↓

allocate dongles

↓

initialize mutexes

↓

initialize condition variables

↓

initialize heaps

↓

create coder threads

↓

create monitor

↓

join everything

↓

cleanup
Phase 10 — Testing

Test every feature independently.

Parser
negative

letters

overflow

scheduler typo
One coder

Should burn out correctly.

Two coders

Compile alternately.

Five coders

No deadlock.

Cooldown

Verify

release

↓

cannot immediately retake
FIFO

Verify order.

EDF

Create deadlines.

Verify earliest deadline always wins.

Completion

Everyone reaches

number_of_compiles_required

Program exits.

Burnout

Verify printed within 10 ms.

Valgrind
no leaks

no invalid read

no invalid write
Suggested File Organization
include/
    codexion.h
    heap.h

src/

main.c

parser/
    parser.c
    parser_utils.c

init/
    init.c
    cleanup.c

time/
    time.c
    sleep.c

heap/
    heap.c
    heap_utils.c

dongle/
    dongle.c
    scheduler.c

coder/
    coder.c

monitor/
    monitor.c

logger/
    logger.c

utils/
    error.c
Algorithms to Use
Problem	Algorithm / Technique	Why
Scheduling	Binary Heap (Priority Queue)	Required by subject, O(log n) insert/remove
FIFO	Heap ordered by arrival time	Deterministic and efficient
EDF	Heap ordered by deadline → arrival time → coder ID	Meets subject requirements and tie-breaking
Synchronization	pthread_mutex_t + pthread_cond_t	Required by subject and avoids busy waiting
Logging	Single mutex	Prevents interleaved output
Burnout detection	Dedicated monitor polling every ~1 ms	Meets the 10 ms precision requirement
Cooldown	Absolute cooldown timestamp + timed waits or rechecks	Prevents immediate reuse while remaining efficient
Deadlock prevention	Coordinated two-dongle acquisition with consistent mutex ordering	Prevents circular wait while preserving fairness
Time complexity	O(log n) queue operations, O(N) monitor scan	Efficient and scalable
Overall development roadmap
Define all data structures and headers.
Implement utilities (time, parser, cleanup, logger).
Implement and thoroughly test the binary heap.
Implement the dongle abstraction with cooldown and scheduling.
Implement coder threads and the compile/debug/refactor cycle.
Implement the monitor thread and simulation termination.
Integrate initialization and cleanup.
Run extensive tests (correctness, timing, FIFO, EDF, cooldown, starvation, memory leaks, thread safety).
Write the README documenting synchronization choices, blocking cases, and AI usage.

This plan provides a complete blueprint that another developer—or another AI—could follow to build a solution that satisfies the subject while remaining maintainable and defensible during a 42 evaluation.
