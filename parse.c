/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 17:03:44 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 19:34:13 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	parse_number(const char *text, long *value)
{
	long	result;

	if (!text || !*text)
		return (0);
	result = 0;
	while (*text == ' ')
		text++;
	if (*text == '+')
		text++;
	while (*text)
	{
		if ((*text < '0') || (*text > '9')
			|| (result > (LONG_MAX - (*text - '0')) / 10))
			return (0);
		result = result * 10 + (*text - '0');
		text++;
	}
	*value = result;
	return (1);
}

static void	set_config(t_config *config, long *values, char *policy)
{
	config->count = (int)values[0];
	config->die_ms = values[1];
	config->compile_ms = values[2];
	config->debug_ms = values[3];
	config->refactor_ms = values[4];
	config->quota = values[5];
	config->cooldown_ms = values[6];
	config->policy = POLICY_FIFO;
	if (!strcmp(policy, "edf"))
		config->policy = POLICY_EDF;
}

static int	validate_values(long *values)
{
	if (values[0] < 1 || values[0] > INT_MAX || values[1] < 1
		|| values[2] < 1 || values[3] < 0 || values[4] < 0
		|| values[5] < 0 || values[6] < 0)
		return (0);
	return (1);
}

int	parse_config(int argc, char **argv, t_config *config)
{
	long	values[7];
	int		i;

	if (argc != 9)
		return (0);
	i = 0;
	while (i < 7)
	{
		if (!parse_number(argv[i + 1], &values[i]))
			return (0);
		i++;
	}
	if (!validate_values(values))
		return (0);
	if (strcmp(argv[8], "fifo") && strcmp(argv[8], "edf"))
		return (0);
	set_config(config, values, argv[8]);
	return (1);
}
