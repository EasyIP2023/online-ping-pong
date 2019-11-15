#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

#include <common.h>

static const char *term_colors[] = {
	[PP_NONE]    = "",
	[PP_SUCCESS] = "\x1B[32;1m",
	[PP_DANGER]  = "\x1B[31;1m",
	[PP_INFO] 	 = "\x1B[30;1m",
	[PP_WARNING] = "\x1B[33;1m",
	[PP_RESET]   = "\x1b[0m",
};

void _pp_log_me(pp_log_type type, FILE *stream, const char *fmt, ...) {
	char buffer[26];
	va_list args; /* type that holds variable arguments */

	/* create message time stamp */
	struct tm result;
	time_t t = time(NULL);
	struct tm *tm_info = localtime_r(&t, &result);

	/* generate time */
	strftime(buffer, sizeof(buffer), "%F %T - ", tm_info);
	fprintf(stream, "%s", buffer);

	unsigned c = (type < PP_MAX_LOG_ENUM) ? type : PP_MAX_LOG_ENUM - 1;

	fprintf(stream, "%s", term_colors[c]);

	va_start(args, fmt);
	vfprintf(stream, fmt, args);
	va_end(args);

	/* Reset terminal color */
	fprintf(stream, "%s\n", term_colors[PP_RESET]);
}

/* Modified version of what's in wlroots */
const char *_pp_strip_path(const char *filepath) {
	if (*filepath == '.')
		while (*filepath == '.' || *filepath == '/')
			filepath++;
	return filepath;
}
