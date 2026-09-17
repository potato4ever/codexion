/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 17:03:33 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 19:26:03 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	simulation_stopped(t_sim *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->state_mutex);
	stopped = sim->stopped;
	pthread_mutex_unlock(&sim->state_mutex);
	return (stopped);
}

void	run_simulation(t_sim *sim, int count)
{
	start_simulation(sim);
	join_coders(sim, count);
	pthread_join(sim->monitor, NULL);
	simulation_destroy(sim);
}

void	join_coders(t_sim *sim, int count)
{
	while (count-- > 0)
		pthread_join(sim->coders[count].thread, NULL);
}

int	create_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, worker_main,
				&sim->coders[i]) != 0)
			return (i);
		i++;
	}
	return (i);
}
int	main(int argc, char **argv)
{
	t_config	config;
	t_sim		sim;
	int			count;
	int			status;

	status = init_simulation(argc, argv, &config, &sim);
	if (status == 0)
		return (status);
	count = create_coders(&sim);
	if (count != config.number_of_coders
		|| pthread_create(&sim.monitor, NULL, monitor_main, &sim) != 0)
	{
		cleanup_thread_error(&sim, count);
		fprintf(stderr, "Error: thread creation failed\n");
		return (1);
	}
	run_simulation(&sim, count);
	return (0);
}
