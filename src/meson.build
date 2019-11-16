
sdl2 = dependency('sdl2')
threads = dependency('threads')

ping_pong_files = [
	'log.c', 'setup.c',
	'screen.c', 'main.c'
]

lib_ping_pong = static_library(
	'lpp',
	files(ping_pong_files),
	include_directories: pp_inc,
	dependencies: [sdl2, threads]
)
