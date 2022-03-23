# Runhouse
Solving running race problem using message buffers.

This program simulates interaction between several different processes: one 'Judge' process and N 'Runners' processes. Processes interact via message buffers in a following way: when all the Runners have sent sent the readiness message, Judge gives a signal of a race starting; then Runners sleep for some time and send messages upon waiting for long enough. When all Runners have completed, Judge gives a signal of finishing the race.
