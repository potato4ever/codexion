#include "codexion.h"

void	*monitor_main(void *arg)
{
	t_sim			*sim;
	long			now;
	long			soonest;
	int			victim;
	int			i;
	struct timespec	deadline;

	sim = (t_sim *)arg;
	pthread_mutex_lock(&sim->state_mutex);
	while (!sim->start_ready && !sim->stopped)
		pthread_cond_wait(&sim->event, &sim->state_mutex);
	while (!sim->stopped)
	{
		now = now_ms();
		soonest = LONG_MAX;
		victim = -1;
		i = 0;
		while (i < sim->config.count)
		{
			if (sim->coders[i].last_start + sim->config.die_ms < soonest)
			{
				soonest = sim->coders[i].last_start + sim->config.die_ms;
				victim = i;
			}
			i++;
		}
		if (now >= soonest + 9)
		{
			sim->stopped = 1;
			sim->burned_id = victim + 1;
			print_event_locked(sim, sim->burned_id, "burned out");
			pthread_cond_broadcast(&sim->event);
			break ;
		}
		ms_to_timespec(soonest, &deadline);
		pthread_cond_timedwait(&sim->event, &sim->state_mutex, &deadline);
	}
	pthread_mutex_unlock(&sim->state_mutex);
	return (NULL);
}
