
threads = dependency('threads')

tpool = ['tpool.c']
net = ['server.c']

lib_pp_net = static_library(
  'lppn',
  files(net, tpool),
  include_directories: pp_inc,
  dependencies: [threads]
)
