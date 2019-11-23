threads = dependency('threads')

net = ['server.c']

deps = [threads]

lib_pp_net = static_library(
	'lppn',
	files(net),
	include_directories: pp_inc,
	dependencies: deps
)
