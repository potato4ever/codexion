/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 17:03:49 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 20:02:09 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	pair_ready_locked(t_sim *sim, t_coder *coder, long now)
{
	int			left;
	int			right;
	int			ready;
	t_dongle	*first;
	t_dongle	*second;

	left = left_dongle(coder->id);
	right = right_dongle(sim, coder->id);
	first = &sim->dongles[left];
	second = &sim->dongles[right];
	lock_pair(sim, left, right);
	ready = (!first->busy && !second->busy && first->cooldown_until <= now
			&& second->cooldown_until <= now);
	unlock_pair(sim, left, right);
	return (ready);
}

static void	grant_pair_locked(t_sim *sim, t_coder *coder, long now)
{
	int			left;
	int			right;
	t_dongle	*first;
	t_dongle	*second;

	left = left_dongle(coder->id);
	right = right_dongle(sim, coder->id);
	first = &sim->dongles[left];
	second = &sim->dongles[right];
	lock_pair(sim, left, right);
	heap_remove(sim, &first->queue, &coder->request);
	heap_remove(sim, &second->queue, &coder->request);
	first->busy = 1;
	second->busy = 1;
	unlock_pair(sim, left, right);
	coder->request.queued = 0;
	coder->request.owns_pair = 1;
	coder->last_start = now;
	coder->request.deadline = now + sim->config.die_ms;
}

void	try_schedule_locked(t_sim *sim)
{
	t_coder	*winner;
	long	now;
	int		i;

	if (sim->stopped)
		return ;
	while (1)
	{
		now = now_ms();
		winner = NULL;
		i = 0;
		while (i < sim->config.count)
		{
			if (sim->coders[i].request.queued && pair_ready_locked(sim,
					&sim->coders[i], now) && (!winner || request_before(sim,
						&sim->coders[i].request, &winner->request)))
				winner = &sim->coders[i];
			i++;
		}
		if (!winner)
			break ;
		grant_pair_locked(sim, winner, now);
	}
}

void	request_pair_locked(t_coder *coder)
{
	int			left;
	int			right;
	t_sim		*sim;
	t_dongle	*first;
	t_dongle	*second;

	sim = coder->sim;
	if (sim->config.count > 1)
	{
		left = left_dongle(coder->id);
		right = right_dongle(sim, coder->id);
		first = &sim->dongles[left];
		second = &sim->dongles[right];
		coder->request.sequence = sim->next_sequence++;
		coder->request.deadline = coder->last_start + sim->config.die_ms;
		coder->request.queued = 1;
		lock_pair(sim, left, right);
		heap_push(sim, &first->queue, &coder->request);
		heap_push(sim, &second->queue, &coder->request);
		unlock_pair(sim, left, right);
	}
}

void	release_pair_locked(t_coder *coder)
{
	t_sim	*sim;
	int		left;
	int		right;
	long	now;

	if (!coder->request.owns_pair)
		return ;
	sim = coder->sim;
	left = left_dongle(coder->id);
	right = right_dongle(sim, coder->id);
	now = now_ms();
	release_dongles(sim, left, right, now);
	coder->request.owns_pair = 0;
	try_schedule_locked(sim);
}
