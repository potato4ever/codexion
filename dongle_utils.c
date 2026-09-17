/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 19:22:13 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 19:22:14 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	lock_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
}

void	unlock_dongle(t_dongle *dongle)
{
	pthread_mutex_unlock(&dongle->mutex);
}

void	lock_pair(t_sim *sim, int left, int right)
{
	if (left == right)
	{
		lock_dongle(&sim->dongles[left]);
		return ;
	}
	if (left < right)
	{
		lock_dongle(&sim->dongles[left]);
		lock_dongle(&sim->dongles[right]);
	}
	else
	{
		lock_dongle(&sim->dongles[right]);
		lock_dongle(&sim->dongles[left]);
	}
}

void	unlock_pair(t_sim *sim, int left, int right)
{
	if (left == right)
	{
		unlock_dongle(&sim->dongles[left]);
		return ;
	}
	if (left < right)
	{
		unlock_dongle(&sim->dongles[right]);
		unlock_dongle(&sim->dongles[left]);
	}
	else
	{
		unlock_dongle(&sim->dongles[left]);
		unlock_dongle(&sim->dongles[right]);
	}
}
