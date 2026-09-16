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
is the burnout time is it consistent with is compiling state





what the difference between usleep and cond_wait
what is thread
what the difference between it and process
how these works and what they takes:
  pthread_create()
  pthread_mutex*()
  pthread_cond*()
  gettimeofday()
  timeval
#define for (safety reasons)
should i care about something like overflow like in this line 
- (coder->request.deadline = coder->last_start + sim->config.die_ms;)
deadline is long it can overflow at a specific point through multiple ways like giving it int max and an int max in number of compile there is a potential
why we lock dongles like here we are just reading it 
- (ready = (!first->busy && !second->busy && first->cooldown_until <= now
			&& second->cooldown_until <= now);)
asumption: busy is main reason cuz it variable i assume that cooldown is not cuz if it 60 it gonna be always 60 but if it change and that what it look like then it also a reason to use mutex
is the order of unlocking dongles matter
