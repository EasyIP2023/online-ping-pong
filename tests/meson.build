check = dependency('check')

extra_args = ['-g', '-ggdb3']

test_dir = meson.current_build_dir()


# Testing vulkan suite
ppg_bmp_test = executable('ppg-bmp-test',
  'test-bmp.c',
  include_directories : pp_inc,
  dependencies : [check],
  link_with : lib_ping_pong,
  link_args : extra_args,
  install : false
)

run_command('cp', '../bmps/background.bmp', '../bmps/image.bmp', test_dir)

test('ppg_bmp_test', ppg_bmp_test, suite: ['all'])
