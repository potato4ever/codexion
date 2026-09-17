/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_initializer.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 19:34:48 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 19:37:25 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_dongles(t_sim *sim, int *initialized)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
			break ;
		if (!heap_init(&sim->dongles[i].queue, sim->config.number_of_coders))
		{
			i++;
			break ;
		}
		i++;
	}
	*initialized = i;
	return (i == sim->config.number_of_coders);
}

int	init_global_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->state_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&sim->print_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->state_mutex);
		return (0);
	}
	if (pthread_cond_init(&sim->event, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->print_mutex);
		pthread_mutex_destroy(&sim->state_mutex);
		return (0);
	}
	return (1);
}

int	init_memory(t_sim *sim, t_config *config)
{
	sim->dongles = malloc(sizeof(*sim->dongles)
			* (size_t)config->number_of_coders);
	if (!sim->dongles)
		return (0);
	memset(sim->dongles, 0, sizeof(*sim->dongles)
		* (size_t)config->number_of_coders);
	sim->coders = malloc(sizeof(*sim->coders)
			* (size_t)config->number_of_coders);
	if (!sim->coders)
	{
		free(sim->dongles);
		return (0);
	}
	memset(sim->coders, 0, sizeof(*sim->coders)
		* (size_t)config->number_of_coders);
	return (1);
}

void	init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].sim = sim;
		sim->coders[i].request.coder_id = i + 1;
		i++;
	}
}

int	simulation_init(t_sim *sim, t_config *config)
{
	int	initialized;

	memset(sim, 0, sizeof(*sim));
	sim->config = *config;
	if (!init_memory(sim, config))
		return (0);
	if (!init_global_mutexes(sim))
	{
		free(sim->coders);
		free(sim->dongles);
		return (0);
	}
	if (!init_dongles(sim, &initialized))
	{
		cleanup_init_failure(sim, initialized);
		return (0);
	}
	sim->next_sequence = 1;
	init_coders(sim);
	return (1);
}
