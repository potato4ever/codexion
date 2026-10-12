/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 19:55:42 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 19:55:43 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	wait_until_event(t_coder *coder)
{
	t_sim	*sim;
	long	soonest;

	sim = coder->sim;
	while (!sim->stopped && !coder->request.owns_pair)
	{
		if (try_acquire_self_locked(coder))
			break ;
		soonest = find_soonest_cooldown(sim);
		wait_for_cooldown(sim, soonest);
	}
	return (!sim->stopped && coder->request.owns_pair);
}

int	sleep_until(t_coder *coder, long end)
{
	t_sim	*sim;
	long	remaining;

	sim = coder->sim;
	while (!simulation_stopped(sim))
	{
		remaining = end - now_ms();
		if (remaining <= 0)
			break ;
		if (remaining > 1)
			usleep(1000);
		else
			usleep((useconds_t)remaining * 1000);
	}
	return (!simulation_stopped(sim) && now_ms() >= end);
}

int	interruptible_sleep(t_coder *coder, long duration)
{
	return (sleep_until(coder, now_ms() + duration));
}

int	finish_compile_at_grant_time(t_coder *coder)
{
	long	end;

	pthread_mutex_lock(&coder->sim->state_mutex);
	end = coder->last_start + coder->sim->config.time_to_compile;
	pthread_mutex_unlock(&coder->sim->state_mutex);
	if (!sleep_until(coder, end))
		return (0);
	return (1);
}

int	finish_compile(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_mutex);
	if (sim->stopped)
	{
		release_pair_locked(coder);
		pthread_mutex_unlock(&sim->state_mutex);
		return (0);
	}
	coder->compiles++;
	release_pair_locked(coder);
	pthread_mutex_unlock(&sim->state_mutex);
	return (1);
}
