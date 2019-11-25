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
    # Added these to preprocessor constants just in case
    '-DPPG_VERSION="@0@"'.format(meson.project_version()),
    '-DPPG_DIR_SRC="@0@"'.format(meson.current_source_dir()),
    '-D_DEFAULT_SOURCE'
  ],
  language : 'c'
)

pp_inc = include_directories('.','include')

subdir('src')
subdir('tests')

executable(
  'online-ping-pong',
  link_with: [lib_pp, lib_pp_std, lib_pp_net, lib_pp_cli_exec],
  install: true,
  link_args : extra_args,
  install_dir: '/usr/bin'
)

executable(
  'oppg-server-start',
  link_with: [lib_pp, lib_pp_std, lib_pp_net, lib_pp_serv_exec],
  install: true,
  link_args : extra_args,
  install_dir: '/usr/bin'
)
