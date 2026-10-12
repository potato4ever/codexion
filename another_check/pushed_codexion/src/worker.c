/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 17:03:58 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 17:04:00 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	wait_for_start_coder(t_coder *coder)
{
	t_sim	*sim;
	long	delay;
	long	elapsed;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_mutex);
	while (!sim->start_ready && !sim->stopped)
		pthread_cond_wait(&sim->event, &sim->state_mutex);
	pthread_mutex_unlock(&sim->state_mutex);
	delay = (sim->config.time_to_compile + sim->config.dongle_cooldown) / 2;
	elapsed = 0;
	while (coder->id % 2 == 0 && elapsed < delay && !simulation_stopped(sim))
	{
		usleep(1000);
		elapsed++;
	}
}

long	find_soonest_cooldown(t_sim *sim)
{
	long	soonest;
	long	now;
	int		i;

	soonest = LONG_MAX;
	now = now_ms();
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		if (sim->dongles[i].cooldown_until > now
			&& sim->dongles[i].cooldown_until < soonest)
			soonest = sim->dongles[i].cooldown_until;
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
	return (soonest);
}

void	wait_for_cooldown(t_sim *sim, long soonest)
{
	long	remaining;

	pthread_mutex_unlock(&sim->state_mutex);
	remaining = soonest - now_ms();
	if (soonest == LONG_MAX || remaining > 1)
		usleep(1000);
	else if (remaining > 0)
		usleep((useconds_t)remaining * 1000);
	pthread_mutex_lock(&sim->state_mutex);
}

void	*worker_main(void *arg)
{
	t_coder	*coder;
	t_sim	*sim;

	coder = (t_coder *)arg;
	sim = coder->sim;
	wait_for_start_coder(coder);
	while (!simulation_stopped(sim))
	{
		if (!run_cycle(coder))
			break ;
	}
	pthread_mutex_lock(&sim->state_mutex);
	release_pair_locked(coder);
	pthread_mutex_unlock(&sim->state_mutex);
	return (NULL);
}
