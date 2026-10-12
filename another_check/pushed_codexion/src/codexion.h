/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 17:03:14 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/17 14:56:41 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
}	t_policy;

typedef struct s_config
{
	int			number_of_coders;
	long		time_to_burnout;
	long		time_to_compile;
	long		time_to_debug;
	long		time_to_refactor;
	long		number_of_compiles_required;
	long		dongle_cooldown;
	t_policy	scheduler;
}	t_config;

typedef struct s_request
{
	int					coder_id;
	int					queued;
	int					owns_pair;
	long				deadline;
	unsigned long		sequence;
}	t_request;

typedef struct s_heap
{
	t_request	**items;
	int			len;
	int			cap;
}	t_heap;

typedef struct s_dongle
{
	int				busy;
	long			cooldown_until;
	pthread_mutex_t	mutex;
	t_heap			queue;
}	t_dongle;

struct	s_sim;

typedef struct s_coder
{
	int					id;
	long				last_start;
	long				compiles;
	t_request			request;
	struct s_sim		*sim;
	pthread_t			thread;
}	t_coder;

typedef struct s_sim
{
	int				start_ready;
	int				initial_requests;
	int				initial_requests_ready;
	int				stopped;
	int				burned_id;
	t_config		config;
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_mutex_t	state_mutex;
	pthread_mutex_t	print_mutex;
	pthread_cond_t	event;
	pthread_t		monitor;
	long			start_ms;
	unsigned long	next_sequence;
}	t_sim;

int			parse_config(int argc, char **argv, t_config *config);
int			check_monitor(t_sim *sim, long soonest, int victim);
int			init_dongles(t_sim *sim, int *initialized);
int			left_dongle(int id);
int			right_dongle(t_sim *sim, int id);
int			run_cycle(t_coder *coder);
int			simulation_stopped(t_sim *sim);
int			heap_init(t_heap *heap, int capacity);
int			heap_push(t_sim *sim, t_heap *heap, t_request *request);
int			request_before(t_sim *sim, t_request *a, t_request *b);
int			simulation_init(t_sim *sim, t_config *config);
int			finish_compile(t_coder *coder);
int			interruptible_sleep(t_coder *coder, long duration);
int			finish_compile_at_grant_time(t_coder *coder);
int			sleep_until(t_coder *coder, long end);
int			create_coders(t_sim *sim);
int			all_finished_locked(t_sim *sim);
int			try_acquire_self_locked(t_coder *coder);
int			init_memory(t_sim *sim, t_config *config);
int			init_global_mutexes(t_sim *sim);
int			wait_until_event(t_coder *coder);
int			init_simulation(int argc, char **argv, t_config *config,
				t_sim *sim);
long		now_ms(void);
void		join_coders(t_sim *sim, int count);
void		cleanup_thread_error(t_sim *sim, int count);
void		run_simulation(t_sim *sim, int count);
void		heap_destroy(t_heap *heap);
void		unlock_pair(t_sim *sim, int left, int right);
void		lock_pair(t_sim *sim, int left, int right);
void		unlock_dongle(t_dongle *dongle);
void		lock_dongle(t_dongle *dongle);
t_request	*heap_peek(t_heap *heap);
t_request	*heap_pop(t_sim *sim, t_heap *heap);
void		simulation_destroy(t_sim *sim);
void		print_event_locked(t_sim *sim, int id, const char *event);
void		request_pair_locked(t_coder *coder);
void		release_pair_locked(t_coder *coder);
void		*worker_main(void *arg);
void		*monitor_main(void *arg);
void		swap_request(t_request **a, t_request **b);
void		heap_sift_up(t_sim *sim, t_heap *heap, int index);
void		heap_sift_down(t_sim *sim, t_heap *heap, int index);
void		init_coders(t_sim *sim);
void		destroy_dongles(t_sim *sim, int count);
void		cleanup_init_failure(t_sim *sim, int initialized);
void		monitor_sleep(long remaining);
void		wait_for_start(t_sim *sim);
void		start_simulation(t_sim *sim);
void		stop_simulation(t_sim *sim);
void		release_dongles(t_sim *sim, int left, int right, long now);
long		find_soonest_cooldown(t_sim *sim);
void		wait_for_cooldown(t_sim *sim, long soonest);
void		log_state(t_coder *coder, const char *message);

#endif
