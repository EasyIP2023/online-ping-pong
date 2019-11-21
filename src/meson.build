
sdl2 = dependency('sdl2')
sdl2_image = dependency('SDL2_image')
sdl2_ttf = dependency('SDL2_ttf')
sdl2_mixer = dependency('SDL2_mixer')
threads = dependency('threads')

pp_files = [
  'pong/screen.c', 'pong/events.c',
  'pong/ball.c', 'pong/paddle.c',
  'pong/audio.c'
]

exec_files = ['log.c', 'setup.c', 'main.c']
deps = [sdl2, sdl2_image, sdl2_ttf, sdl2_mixer, threads]

lib_ping_pong = static_library(
	'lpp',
	files(pp_files, exec_files),
	include_directories: pp_inc,
	dependencies: deps
)
