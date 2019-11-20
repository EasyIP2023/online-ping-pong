
sdl2 = dependency('sdl2')
sdl2_image = dependency('SDL2_image')
threads = dependency('threads')

pp_files = [
  'pong/screen.c', 'pong/events.c',
  'pong/ball.c', 'pong/paddle.c'
]

exec_files = ['log.c', 'setup.c', 'main.c']

lib_ping_pong = static_library(
	'lpp',
	files(pp_files, exec_files),
	include_directories: pp_inc,
	dependencies: [sdl2, sdl2_image, threads]
)
