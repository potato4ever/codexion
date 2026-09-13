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

void	wait_for_start(t_sim *sim)
{
	pthread_mutex_lock(&sim->state_mutex);
	while (!sim->start_ready && !sim->stopped)
		pthread_cond_wait(&sim->event, &sim->state_mutex);
	pthread_mutex_unlock(&sim->state_mutex);
}

void	monitor_sleep(long remaining)
{
	if (remaining > 1)
		usleep(1000);
	else if (remaining > 0)
		usleep((useconds_t)remaining * 1000);
}
