project(
	'online-ping-pong',
	'c',
	version: '1.0.0',
	license: 'MIT',
	meson_version: '>=0.52.0',
	default_options: [
		'c_std=c11',
		'warning_level=2',
		'werror=true',
	],
)

cc = meson.get_compiler('c')

add_project_arguments(
	[
		'-DLUCUR_VERSION="@0@"'.format(meson.project_version()),
		'-DWLU_DIR_SRC="@0@"'.format(meson.current_source_dir()),
		'-D_POSIX_C_SOURCE=200809L'
	],
	language : 'c'
)

pp_inc = include_directories('.','include')

subdir('src')

executable(
	'online-ping-pong',
	link_with: lib_ping_pong,
	install: true,
	install_dir: '/usr/bin'
)
