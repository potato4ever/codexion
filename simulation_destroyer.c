/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_destroyer.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 19:34:42 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 19:34:43 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_dongles(t_sim *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		heap_destroy(&sim->dongles[i].queue);
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		i++;
	}
}

void	cleanup_init_failure(t_sim *sim, int initialized)
{
	destroy_dongles(sim, initialized);
	pthread_cond_destroy(&sim->event);
	pthread_mutex_destroy(&sim->print_mutex);
	pthread_mutex_destroy(&sim->state_mutex);
	free(sim->coders);
	free(sim->dongles);
}

void	simulation_destroy(t_sim *sim)
{
	destroy_dongles(sim, sim->config.count);
	pthread_cond_destroy(&sim->event);
	pthread_mutex_destroy(&sim->print_mutex);
	pthread_mutex_destroy(&sim->state_mutex);
	free(sim->dongles);
	free(sim->coders);
}
