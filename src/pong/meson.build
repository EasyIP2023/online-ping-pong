
sdl2 = dependency('sdl2')
sdl2_image = dependency('SDL2_image')
sdl2_ttf = dependency('SDL2_ttf')
sdl2_mixer = dependency('SDL2_mixer')

pp_files = [
  'screen.c', 'events.c',
  'player.c', 'audio.c',
  'ball.c'
]

deps = [sdl2, sdl2_image, sdl2_ttf, sdl2_mixer]

lib_pp = static_library(
	'lpp',
	files(pp_files),
	include_directories: pp_inc,
	dependencies: deps
)
