/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 19:55:36 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 19:55:37 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wait_for_start(t_coder *coder)
{
	t_sim	*sim;
	long	delay;

	sim = coder->sim;
	delay = sim->config.compile_ms + sim->config.cooldown_ms - 10;
	pthread_mutex_lock(&sim->state_mutex);
	while (!sim->start_ready && !sim->stopped)
		pthread_cond_wait(&sim->event, &sim->state_mutex);
	pthread_mutex_unlock(&sim->state_mutex);
	if (coder->id % 2 == 0 && delay > 0)
		usleep((useconds_t)delay * 1000);
}

int	run_cycle(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_mutex);
	request_pair_locked(coder);
	if (!wait_until_event(coder))
	{
		pthread_mutex_unlock(&sim->state_mutex);
		return (0);
	}
	pthread_mutex_unlock(&sim->state_mutex);
	log_state(coder, "has taken a dongle");
	log_state(coder, "has taken a dongle");
	log_state(coder, "is compiling");
	if (!finish_compile_at_grant_time(coder))
		return (0);
	if (!finish_compile(coder))
		return (0);
	log_state(coder, "is debugging");
	if (!interruptible_sleep(coder, sim->config.debug_ms))
		return (0);
	log_state(coder, "is refactoring");
	if (!interruptible_sleep(coder, sim->config.refactor_ms))
		return (0);
	return (1);
}
