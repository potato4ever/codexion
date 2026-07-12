#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <limits.h>

typedef enum e_policy
{
	POLICY_FIFO,
	POLICY_EDF
}t_policy;

typedef struct s_config
{
	int		count;
	long		die_ms;
	long		compile_ms;
	long		debug_ms;
	long		refactor_ms;
	long		quota;
	long		cooldown_ms;
	t_policy	policy;
}t_config;

typedef struct s_request
{
	int			coder_id;
	unsigned long	sequence;
	long			deadline;
	int			queued;
	int			owns_pair;
}t_request;

typedef struct s_heap
{
	t_request	**items;
	int			len;
	int			cap;
}t_heap;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	t_heap			queue;
	int			busy;
	long			cooldown_until;
}t_dongle;

struct s_sim;

typedef struct s_coder
{
	int				id;
	long				last_start;
	long				compiles;
	t_request			request;
	struct s_sim		*sim;
	pthread_t		thread;
}t_coder;

typedef struct s_sim
{
	t_config		config;
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_mutex_t	state_mutex;
	pthread_mutex_t	print_mutex;
	pthread_cond_t	event;
	pthread_t		monitor;
	long			start_ms;
	unsigned long	next_sequence;
	int			start_ready;
	int			stopped;
	int			burned_id;
}t_sim;

int		parse_config(int argc, char **argv, t_config *config);
long		now_ms(void);
void		ms_to_timespec(long target_ms, struct timespec *ts);
int		heap_init(t_heap *heap, int capacity);
void		heap_destroy(t_heap *heap);
int		heap_push(t_sim *sim, t_heap *heap, t_request *request);
t_request	*heap_peek(t_heap *heap);
t_request	*heap_pop(t_sim *sim, t_heap *heap);
int		request_before(t_sim *sim, t_request *a, t_request *b);
int		simulation_init(t_sim *sim, t_config *config);
void		simulation_destroy(t_sim *sim);
void		print_event_locked(t_sim *sim, int id, const char *event);
void		request_pair_locked(t_coder *coder);
void		try_schedule_locked(t_sim *sim);
void		release_pair_locked(t_coder *coder);
int		simulation_stopped(t_sim *sim);
void		*worker_main(void *arg);
void		*monitor_main(void *arg);

#endif
