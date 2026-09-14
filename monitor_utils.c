/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 19:27:45 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 19:27:46 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	all_finished_locked(t_sim *sim)
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

int	check_monitor(t_sim *sim, long soonest, int victim)
{
	if (all_finished_locked(sim))
	{
		sim->stopped = 1;
		pthread_cond_broadcast(&sim->event);
		return (1);
	}
	if (now_ms() >= soonest)
	{
		sim->stopped = 1;
		sim->burned_id = victim + 1;
		print_event_locked(sim, sim->burned_id, "burned out");
		pthread_cond_broadcast(&sim->event);
		return (1);
	}
	return (0);
}

void	monitor_sleep(long remaining)
{
	if (remaining > 1)
		usleep(1000);
	else if (remaining > 0)
		usleep((useconds_t)remaining * 1000);
}
