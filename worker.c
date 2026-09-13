#include "codexion.h"

static void	log_state(t_coder *coder, const char *message)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_mutex);
	if (!sim->stopped)
		print_event_locked(sim, coder->id, message);
	pthread_mutex_unlock(&sim->state_mutex);
}

static int	wait_until_event(t_coder *coder)
{
	t_sim			*sim;
	long			soonest;
	long			now;
	int				i;
	struct timespec	deadline;

	sim = coder->sim;
	while (!sim->stopped && !coder->request.owns_pair)
	{
		try_schedule_locked(sim);
		if (coder->request.owns_pair)
			break ;
		soonest = LONG_MAX;
		now = now_ms();
		i = 0;
		while (i < sim->config.count)
		{
			pthread_mutex_lock(&sim->dongles[i].mutex);
			if (sim->dongles[i].cooldown_until > now
				&& sim->dongles[i].cooldown_until < soonest)
				soonest = sim->dongles[i].cooldown_until;
			pthread_mutex_unlock(&sim->dongles[i].mutex);
			i++;
		}
		if (soonest == LONG_MAX)
			pthread_cond_wait(&sim->event, &sim->state_mutex);
		else
		{
			ms_to_timespec(soonest, &deadline);
			pthread_cond_timedwait(&sim->event, &sim->state_mutex, &deadline);
		}
	}
	return (!sim->stopped && coder->request.owns_pair);
}

static int	sleep_until(t_coder *coder, long end)
{
	t_sim			*sim;
	struct timespec	deadline;
	int			finished;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_mutex);
	while (!sim->stopped && now_ms() < end)
	{
		ms_to_timespec(end, &deadline);
		pthread_cond_timedwait(&sim->event, &sim->state_mutex, &deadline);
	}
	finished = (!sim->stopped && now_ms() >= end);
	pthread_mutex_unlock(&sim->state_mutex);
	return (finished);
}

static int	interruptible_sleep(t_coder *coder, long duration)
{
	return (sleep_until(coder, now_ms() + duration));
}

static int	finish_compile_at_grant_time(t_coder *coder)
{
	long	end;

	pthread_mutex_lock(&coder->sim->state_mutex);
	end = coder->last_start + coder->sim->config.compile_ms;
	pthread_mutex_unlock(&coder->sim->state_mutex);
	return (sleep_until(coder, end));
}

static int	all_finished_locked(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.count)
	{
		if (sim->coders[i].compiles < sim->config.quota)
			return (0);
		i++;
	}
	return (1);
}

static int	finish_compile(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_mutex);
	if (sim->stopped)
	{
		release_pair_locked(coder);
		pthread_mutex_unlock(&sim->state_mutex);
		return (0);
	}
	coder->compiles++;
	if (all_finished_locked(sim))
	{
		sim->stopped = 1;
		release_pair_locked(coder);
		pthread_cond_broadcast(&sim->event);
		pthread_mutex_unlock(&sim->state_mutex);
		return (0);
	}
	release_pair_locked(coder);
	pthread_mutex_unlock(&sim->state_mutex);
	return (1);
}

void	*worker_main(void *arg)
{
	t_coder	*coder;
	t_sim		*sim;
	long delay;

	coder = (t_coder *)arg;
	sim = coder->sim;
	delay = sim->config.compile_ms + sim->config.cooldown_ms - 10;
	pthread_mutex_lock(&sim->state_mutex);
	while (!sim->start_ready && !sim->stopped)
		pthread_cond_wait(&sim->event, &sim->state_mutex);
	pthread_mutex_unlock(&sim->state_mutex);
	if (coder->id % 2 == 0 && delay > 0)
		usleep(delay * 1000);
	while (!simulation_stopped(sim))
	{
		pthread_mutex_lock(&sim->state_mutex);
		request_pair_locked(coder);
		if (!wait_until_event(coder))
		{
			pthread_mutex_unlock(&sim->state_mutex);
			break ;
		}
		pthread_mutex_unlock(&sim->state_mutex);
		log_state(coder, "has taken a dongle");
		log_state(coder, "has taken a dongle");
		log_state(coder, "is compiling");
		if (!finish_compile_at_grant_time(coder))
			break ;
		if (!finish_compile(coder))
			break ;
		log_state(coder, "is debugging");
		if (!interruptible_sleep(coder, sim->config.debug_ms))
			break ;
		log_state(coder, "is refactoring");
		if (!interruptible_sleep(coder, sim->config.refactor_ms))
			break ;
	}
	pthread_mutex_lock(&sim->state_mutex);
	release_pair_locked(coder);
	pthread_mutex_unlock(&sim->state_mutex);
	return (NULL);
}
