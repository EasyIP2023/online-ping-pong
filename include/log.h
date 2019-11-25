#ifndef LOG_H
#define LOG_H

/* Used to help determine which ANSI Escape Codes to use */
typedef enum ppg_log_type {
  PPG_NONE         = 0x00000000,
  PPG_SUCCESS      = 0x00000001,
  PPG_DANGER       = 0x00000002,
  PPG_INFO         = 0x00000003,
  PPG_WARNING      = 0x00000004,
  PPG_RESET        = 0x00000005,
  PPG_MAX_LOG_ENUM = 0xffffffff
} ppg_log_type;

/**
* function accepts a log type
* a pointer to a file stream
* the fmt of message, any argument you want
*/
void _ppg_log_me(ppg_log_type type, FILE *stream, const char *fmt, ...);
const char *_ppg_strip_path(const char *filepath);

/* Macros defined to help structure message */
#define ppg_log_me(log_type, fmt, ...) \
  _ppg_log_me(log_type, stdout, "[%s:%d] " fmt, _ppg_strip_path(__FILE__), __LINE__, ##__VA_ARGS__)

#define pp_log_err(log_type, fmt, ...) \
  _ppg_log_me(log_type, stderr, "[%s:%d] " fmt, _ppg_strip_path(__FILE__), __LINE__, ##__VA_ARGS__)

#endif
