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

void	log_state(t_coder *coder, const char *message)
{
	t_sim	*sim;

	sim = coder->sim;
	if (!sim->stopped)
		print_event_locked(sim, coder->id, message);
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
	pthread_mutex_lock(&sim->state_mutex);
	log_state(coder, "has taken a dongle");
	log_state(coder, "has taken a dongle");
	log_state(coder, "is compiling");
	pthread_mutex_unlock(&sim->state_mutex);
	if (!finish_compile_at_grant_time(coder))
		return (0);
	if (!finish_compile(coder))
		return (0);
	pthread_mutex_lock(&sim->state_mutex);
	log_state(coder, "is debugging");
	pthread_mutex_unlock(&sim->state_mutex);
	if (!interruptible_sleep(coder, sim->config.debug_ms))
		return (0);
	pthread_mutex_lock(&sim->state_mutex);
	log_state(coder, "is refactoring");
	pthread_mutex_unlock(&sim->state_mutex);
	if (!interruptible_sleep(coder, sim->config.refactor_ms))
		return (0);
	return (1);
}
