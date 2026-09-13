/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 17:03:38 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 17:03:39 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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

static long	find_soonest(t_sim *sim, int *victim)
{
	long	soonest;
	int		i;

	soonest = LONG_MAX;
	i = 0;
	while (i < sim->config.count)
	{
		if (sim->coders[i].last_start + sim->config.die_ms < soonest)
		{
			soonest = sim->coders[i].last_start + sim->config.die_ms;
			*victim = i;
		}
		i++;
	}
	return (soonest);
}

void	*monitor_main(void *arg)
{
	t_sim	*sim;
	long	soonest;
	long	remaining;
	int		victim;

	sim = (t_sim *)arg;
	wait_for_start(sim);
	while (!simulation_stopped(sim))
	{
		pthread_mutex_lock(&sim->state_mutex);
		victim = -1;
		soonest = find_soonest(sim, &victim);
		if (sim->stopped || check_monitor(sim, soonest, victim))
		{
			pthread_mutex_unlock(&sim->state_mutex);
			break ;
		}
		remaining = soonest - now_ms();
		pthread_mutex_unlock(&sim->state_mutex);
		monitor_sleep(remaining);
	}
	return (NULL);
}
