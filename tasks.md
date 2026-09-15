parser:
  spaces in "     234"
  you should + and - if someone put things on "" in the input
what shared between threads 
what the difference between process and thread
what is a mutexes
what is enum in c and how to use it 
what some function get on NULL and what is it + what it get other 
  than null
why we initiaze mutex and cond and any other stuff like this 
why changing this memset(sim->dongles, 0, sizeof(*sim->dongles) * (size_t)config->count); to under the condition can causes leaks simulation.c
valgrind leaks: 
  test if coders and dongles NULL 
  if the mutex and cond fails
helgrind:
  test the same of valgrind 
questions:
 - why i should use memset to initialize what the 
  problem if there is garbage value and why don't 
  using it cause valgrind to complains

# IMPORTANT:
- any functions used should be understood 
- the reason of multiple mutexes 
- why try_schedule_locked called in different places taking in mind that only one thread is gonna served at a time
- why using some stuffs that have vague purpose in the project like heap_remove or even the part of sim->initial_requests_ready
- what happens in case of failure in any part of code
- edit instead of coder_id 
- can you create the same thread multiple times



# TASKS:
reread and understand the logic in :
- the success case 
- in the failure case
- in middle failure
handle the parser
check valgrind and helgrind 
read the subject and the correction sheet
should i handle spaces like atoi "+1    1 1 " = 1 ?
should replace the config names
why removing struct s_sim from line 54 not gonna change anything
solve norminette and add headers
/2 on odd and even
to much wait on sleep when it burnout 

