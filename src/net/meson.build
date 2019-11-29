
threads = dependency('threads')

lib_pp_net = static_library(
  'lppn',
  files('net.c','tpool.c'),
  include_directories: pp_inc,
  dependencies: [threads]
)
