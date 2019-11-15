
sdl2 = dependency('sdl2')

ping_pong_files = [
	'log.c',
  'main.c'
]

lib_ping_pong = static_library(
	'lpp',
	files(ping_pong_files),
	include_directories: pp_inc,
	dependencies: [sdl2]
)
