#include "codexion.h"

static void	stop_simulation(t_sim *sim)
{
	pthread_mutex_lock(&sim->state_mutex);
	sim->stopped = 1;
	pthread_cond_broadcast(&sim->event);
	pthread_mutex_unlock(&sim->state_mutex);
}

static void	start_simulation(t_sim *sim)
{
	int	i;

	pthread_mutex_lock(&sim->state_mutex);
	sim->start_ms = now_ms();
	i = 0;
	while (i < sim->config.count)
	{
		sim->coders[i].last_start = sim->start_ms;
		i++;
	}
	sim->start_ready = 1;
	pthread_cond_broadcast(&sim->event);
	pthread_mutex_unlock(&sim->state_mutex);
}

int	main(int argc, char **argv)
{
	t_config	config;
	t_sim		sim;
	int		i;

	if (!parse_config(argc, argv, &config))
		return (fprintf(stderr, "Error: invalid arguments\n"), 1);
	if (!simulation_init(&sim, &config))
		return (fprintf(stderr, "Error: initialization failed\n"), 1);
	if (config.quota == 0)
		return (simulation_destroy(&sim), 0);
	i = 0;
	while (i < config.count)
	{
		if (pthread_create(&sim.coders[i].thread, NULL, worker_main,
				&sim.coders[i]) != 0)
		{
			stop_simulation(&sim);
			break ;
		}
		i++;
	}
	if (i == config.count && pthread_create(&sim.monitor, NULL,
			&monitor_main, &sim) == 0)
	{
		start_simulation(&sim);
		while (i-- > 0)
			pthread_join(sim.coders[i].thread, NULL);
		pthread_join(sim.monitor, NULL);
	}
	else
	{
		stop_simulation(&sim);
		while (i-- > 0)
			pthread_join(sim.coders[i].thread, NULL);
		fprintf(stderr, "Error: thread creation failed\n");
		simulation_destroy(&sim);
		return (1);
	}
	simulation_destroy(&sim);
	return (0);
}
