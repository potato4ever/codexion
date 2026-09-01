#include "codexion.h"

static int	left_dongle(int id)
{
	return (id - 1);
}

static int	right_dongle(t_sim *sim, int id)
{
	if (id == sim->config.count)
		return (0);
	return (id);
}

static void	lock_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
}

static void	unlock_dongle(t_dongle *dongle)
{
	pthread_mutex_unlock(&dongle->mutex);
}

static void	lock_pair(t_sim *sim, int left, int right)
{
	if (left < right)
	{
		lock_dongle(&sim->dongles[left]);
		lock_dongle(&sim->dongles[right]);
	}
	else
	{
		lock_dongle(&sim->dongles[right]);
		lock_dongle(&sim->dongles[left]);
	}
}

static void	unlock_pair(t_sim *sim, int left, int right)
{
	if (left < right)
	{
		unlock_dongle(&sim->dongles[right]);
		unlock_dongle(&sim->dongles[left]);
	}
	else
	{
		unlock_dongle(&sim->dongles[left]);
		unlock_dongle(&sim->dongles[right]);
	}
}

void	print_event_locked(t_sim *sim, int id, const char *event)
{
	long	stamp;

	stamp = now_ms() - sim->start_ms;
	pthread_mutex_lock(&sim->print_mutex);
	printf("%ld %d %s\n", stamp, id, event);
	pthread_mutex_unlock(&sim->print_mutex);
}

static int	pair_ready_locked(t_sim *sim, t_coder *coder, long now)
{
	int		left;
	int		right;
	t_dongle	*first;
	t_dongle	*second;
	int		ready;

	left = left_dongle(coder->id);
	right = right_dongle(sim, coder->id);
	first = &sim->dongles[left];
	second = &sim->dongles[right];
	lock_pair(sim, left, right);
	ready = (!first->busy && !second->busy && first->cooldown_until <= now
			&& second->cooldown_until <= now);
	unlock_pair(sim, left, right);
	return (ready);
}

static void	grant_pair_locked(t_sim *sim, t_coder *coder, long now)
{
	int		left;
	int		right;
	t_dongle	*first;
	t_dongle	*second;

	left = left_dongle(coder->id);
	right = right_dongle(sim, coder->id);
	first = &sim->dongles[left];
	second = &sim->dongles[right];
	lock_pair(sim, left, right);
	heap_remove(sim, &first->queue, &coder->request);
	heap_remove(sim, &second->queue, &coder->request);
	first->busy = 1;
	second->busy = 1;
	unlock_pair(sim, left, right);
	coder->request.queued = 0;
	coder->request.owns_pair = 1;
	coder->last_start = now;
	coder->request.deadline = now + sim->config.die_ms;
}

void	try_schedule_locked(t_sim *sim)
{
	t_coder	*winner;
	long	now;
	int		i;

	if (sim->stopped)
		return ;
	while (1)
	{
		now = now_ms();
		winner = NULL;
		i = 0;
		while (i < sim->config.count)
		{
			if (sim->coders[i].request.queued && pair_ready_locked(sim,
					&sim->coders[i], now) && (!winner || request_before(sim,
					&sim->coders[i].request, &winner->request)))
				winner = &sim->coders[i];
			i++;
		}
		if (!winner)
			break ;
		grant_pair_locked(sim, winner, now);
	}
	pthread_cond_broadcast(&sim->event);
}

void	request_pair_locked(t_coder *coder)
{
	t_sim		*sim;
	int		left;
	int		right;
	t_dongle	*first;
	t_dongle	*second;

	sim = coder->sim;
	if (sim->config.count > 1)
	{
		left = left_dongle(coder->id);
		right = right_dongle(sim, coder->id);
		first = &sim->dongles[left];
		second = &sim->dongles[right];
		coder->request.sequence = sim->next_sequence++;
		coder->request.deadline = coder->last_start + sim->config.die_ms;
		coder->request.queued = 1;
		lock_pair(sim, left, right);
		heap_push(sim, &first->queue, &coder->request);
		heap_push(sim, &second->queue, &coder->request);
		unlock_pair(sim, left, right);
	}
	if (!sim->initial_requests_ready)
	{
		sim->initial_requests++;
		if (sim->initial_requests == sim->config.count)
			sim->initial_requests_ready = 1;
		else
			return ;
	}
	try_schedule_locked(sim);
}

void	release_pair_locked(t_coder *coder)
{
	t_sim		*sim;
	int		left;
	int		right;
	t_dongle	*first;
	t_dongle	*second;
	long		now;

	if (!coder->request.owns_pair)
		return ;
	sim = coder->sim;
	left = left_dongle(coder->id);
	right = right_dongle(sim, coder->id);
	first = &sim->dongles[left];
	second = &sim->dongles[right];
	now = now_ms();
	lock_pair(sim, left, right);
	first->busy = 0;
	second->busy = 0;
	first->cooldown_until = now + sim->config.cooldown_ms;
	second->cooldown_until = now + sim->config.cooldown_ms;
	unlock_pair(sim, left, right);
	coder->request.owns_pair = 0;
	pthread_cond_broadcast(&sim->event);
	try_schedule_locked(sim);
}

static int	init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.count)
	{
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0
			|| !heap_init(&sim->dongles[i].queue, sim->config.count))
			return (0);
		i++;
	}
	return (1);
}

int	simulation_init(t_sim *sim, t_config *config)
{
	int	i;

	memset(sim, 0, sizeof(*sim));
	sim->config = *config;
	sim->dongles = malloc(sizeof(*sim->dongles) * (size_t)config->count);
	if (!sim->dongles)
		return (0);
	sim->coders = malloc(sizeof(*sim->coders) * (size_t)config->count);
	if (!sim->coders)
		return (0);
  memset(sim->dongles, 0, sizeof(*sim->dongles) * (size_t)config->count);
	if (!sim->dongles || !sim->coders
		|| pthread_mutex_init(&sim->state_mutex, NULL)
		|| pthread_mutex_init(&sim->print_mutex, NULL)
		|| pthread_cond_init(&sim->event, NULL)
		|| !init_dongles(sim))
		return (0);
	memset(sim->coders, 0, sizeof(*sim->coders) * (size_t)config->count);
	sim->next_sequence = 1;
	i = 0;
	while (i < config->count)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].sim = sim;
		sim->coders[i].last_start = 0;
		sim->coders[i].request.coder_id = i + 1;
		i++;
	}
	return (1);
}

void	simulation_destroy(t_sim *sim)
{
	int	i;

	i = 0;
	while (sim->dongles && i < sim->config.count)
	{
		heap_destroy(&sim->dongles[i].queue);
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		i++;
	}
	pthread_cond_destroy(&sim->event);
	pthread_mutex_destroy(&sim->print_mutex);
	pthread_mutex_destroy(&sim->state_mutex);
	free(sim->dongles);
	free(sim->coders);
}

int	simulation_stopped(t_sim *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->state_mutex);
	stopped = sim->stopped;
	pthread_mutex_unlock(&sim->state_mutex);
	return (stopped);
}
