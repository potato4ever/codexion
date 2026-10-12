/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_event.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 19:28:29 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 19:28:29 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_event_locked(t_sim *sim, int id, const char *event)
{
	long	stamp;

	stamp = now_ms() - sim->start_ms;
	pthread_mutex_lock(&sim->print_mutex);
	printf("%ld %d %s\n", stamp, id, event);
	pthread_mutex_unlock(&sim->print_mutex);
}
