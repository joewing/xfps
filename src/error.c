
#include "xfps.h"

void Warning(const char *msg, ...) {

	va_list ap;
	va_start(ap, msg);

	fprintf(stderr, "error: ");
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");

	va_end(ap);

}

