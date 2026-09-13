/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 19:22:56 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 19:22:57 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	left_dongle(int id)
{
	return (id - 1);
}

int	right_dongle(t_sim *sim, int id)
{
	if (id == sim->config.count)
		return (0);
	return (id);
}

void	release_dongles(t_sim *sim, int left, int right, long now)
{
	lock_pair(sim, left, right);
	sim->dongles[left].busy = 0;
	sim->dongles[right].busy = 0;
	sim->dongles[left].cooldown_until = now + sim->config.cooldown_ms;
	sim->dongles[right].cooldown_until = now + sim->config.cooldown_ms;
	unlock_pair(sim, left, right);
}
