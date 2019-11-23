subdir('net')
subdir('pong')

main_files = ['log.c', 'setup.c', 'main.c']

lib_pp_exec = static_library(
	'lppe',
	files(main_files),
	include_directories: pp_inc,
	dependencies: deps
)
