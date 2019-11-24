
subdir('net')
subdir('pong')

std_files = ['log.c', 'setup.c']

lib_pp_std = static_library('lpp_std', files(std_files), include_directories: pp_inc)
lib_pp_cli_exec = static_library('lpp_ce', files('client.c'), include_directories: pp_inc)
lib_pp_serv_exec = static_library('lpp_se', files('server.c'), include_directories: pp_inc)
