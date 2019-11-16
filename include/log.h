#ifndef LOG_H
#define LOG_H

typedef enum pp_log_type {
  PP_NONE = 0,
  PP_SUCCESS = 1,
  PP_DANGER = 2,
  PP_INFO = 3,
  PP_WARNING = 4,
  PP_RESET = 5,
  PP_MAX_LOG_ENUM
} pp_log_type;

void _pp_log_me(pp_log_type type, FILE *stream, const char *fmt, ...);
char *_pp_strip_path(char *filepath);
char *pp_file_path(char *filename);

#define pp_log_me(log_type, fmt, ...) \
  _pp_log_me(log_type, stdout, "[%s:%d] " fmt, _pp_strip_path(__FILE__), __LINE__, ##__VA_ARGS__)

#define pp_log_err(log_type, fmt, ...) \
  _pp_log_me(log_type, stderr, "[%s:%d] " fmt, _pp_strip_path(__FILE__), __LINE__, ##__VA_ARGS__)

#endif
