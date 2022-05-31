/* librist. Copyright © 2019 SipRadius LLC. All right reserved.
 * Author: Sergio Ammirata, Ph.D. <sergio@ammirata.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "stdio-shim.h"
#include <stdlib.h>
#include <limits.h>

#ifdef _MSC_VER
int vasprintf(char** strp, const char* fmt, va_list ap)
{
#ifdef _WIN32
	int size = _vscprintf(fmt, ap);
#else
	// https://stackoverflow.com/a/4785411
	va_list apcopy;
	va_copy(apcopy, ap);
	int size = vsnprintf(NULL, 0, fmt, apcopy);
	va_end(apcopy);
#endif
	if ((size < 0) || (size >= INT_MAX)) {
		goto fail;
	}

	char *str = malloc(size + 1); //+1 for null
	if (str == NULL) {
		goto fail;
	}

	int r = vsnprintf(str, size + 1, fmt, ap);  //+1 for null
	if ((r < 0) || (r > size)) {
		free(str);
		goto fail;
	}

	*strp = str;
	return r;

	fail:
	*strp = NULL;
	return -1;
}

int asprintf(char** strp, const char* fmt, ...)
{
	int r;
	va_list ap;
	va_start(ap, fmt);
	r = vasprintf(strp, fmt, ap);
	va_end(ap);
	return r;
}
#endif
