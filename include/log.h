#ifndef LOG_H
#define LOG_H

typedef enum pp_log_type {
  PPG_NONE = 0,
  PPG_SUCCESS = 1,
  PPG_DANGER = 2,
  PPG_INFO = 3,
  PPG_WARNING = 4,
  PPG_RESET = 5,
  PPG_MAX_LOG_ENUM
} pp_log_type;

void _ppg_log_me(pp_log_type type, FILE *stream, const char *fmt, ...);
char *_ppg_strip_path(char *filepath);
char *ppg_file_path(char *filename);

#define ppg_log_me(log_type, fmt, ...) \
  _ppg_log_me(log_type, stdout, "[%s:%d] " fmt, _ppg_strip_path(__FILE__), __LINE__, ##__VA_ARGS__)

#define pp_log_err(log_type, fmt, ...) \
  _ppg_log_me(log_type, stderr, "[%s:%d] " fmt, _ppg_strip_path(__FILE__), __LINE__, ##__VA_ARGS__)

#endif
