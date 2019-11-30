
subdir('net')
subdir('pong')

lib_pp_std = static_library('lpp_std', files('utils/log.c','utils/setup.c'), include_directories: pp_inc)
lib_pp_exec = static_library('lpp_se', files('main.c','client.c','server.c'), include_directories: pp_inc)
