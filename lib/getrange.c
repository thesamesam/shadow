/*
 * SPDX-FileCopyrightText: 2008       , Nicolas François
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <config.h>

#ident "$Id: $"

#include <ctype.h>
#include <stdlib.h>

#include "defines.h"
#include "prototypes.h"


/*
 * Parse a range and indicate if the range is valid.
 * Valid ranges are in the form:
 *     <long>          -> min=max=long         has_min  has_max
 *     -<long>         -> max=long            !has_min  has_max
 *     <long>-         -> min=long             has_min !has_max
 *     <long1>-<long2> -> min=long1 max=long2  has_min  has_max
 */
int
getrange(const char *range,
         unsigned long *min, bool *has_min,
         unsigned long *max, bool *has_max)
{
	char *endptr;
	unsigned long n;

	if (NULL == range)
		return -1;

	if ('-' == range[0]) {
		if (!isdigit(range[1]))
			return -1;

		errno = 0;
		n = strtoul(&range[1], &endptr, 10);
		if (('\0' != *endptr) || (0 != errno))
			return -1;

		/* -<long> */
		*has_min = false;
		*has_max = true;
		*max = n;
	} else {
		errno = 0;
		n = strtoul(range, &endptr, 10);
		if (endptr == range || 0 != errno)
			return -1;

		switch (*endptr) {
		case '\0':
			/* <long> */
			*has_min = true;
			*has_max = true;
			*min = n;
			*max = n;
			break;
		case '-':
			endptr++;
			if ('\0' == *endptr) {
				/* <long>- */
				*has_min = true;
				*has_max = false;
				*min = n;
			} else if (!isdigit (*endptr)) {
				return -1;
			} else {
				*has_min = true;
				*min = n;
				errno = 0;
				n = strtoul(endptr, &endptr, 10);
				if ('\0' != *endptr || 0 != errno)
					return -1;

				/* <long>-<long> */
				*has_max = true;
				*max = n;
			}
			break;
		default:
			return -1;
		}
	}

	return 0;
}
