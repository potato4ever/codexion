/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 18:08:40 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 18:08:42 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	stop_simulation(t_sim *sim)
{
	pthread_mutex_lock(&sim->state_mutex);
	sim->stopped = 1;
	pthread_cond_broadcast(&sim->event);
	pthread_mutex_unlock(&sim->state_mutex);
}

void	start_simulation(t_sim *sim)
{
	int	i;

	pthread_mutex_lock(&sim->state_mutex);
	sim->start_ms = now_ms();
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->coders[i].last_start = sim->start_ms;
		i++;
	}
	sim->start_ready = 1;
	pthread_cond_broadcast(&sim->event);
	pthread_mutex_unlock(&sim->state_mutex);
}

void	cleanup_thread_error(t_sim *sim, int count)
{
	stop_simulation(sim);
	join_coders(sim, count);
	simulation_destroy(sim);
}

int	init_simulation(int argc, char **argv, t_config *config, t_sim *sim)
{
	if (!parse_config(argc, argv, config))
	{
		fprintf(stderr, "Error: invalid arguments\n");
		return (0);
	}
	if (!simulation_init(sim, config))
	{
		fprintf(stderr, "Error: initialization failed\n");
		return (0);
	}
	if (config->number_of_compiles_required == 0)
	{
		simulation_destroy(sim);
		return (0);
	}
	return (1);
}
