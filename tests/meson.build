check = dependency('check')

extra_args = ['-g', '-ggdb3']

test_dir = meson.current_build_dir()

# Testing server suite
ppg_net_test = executable('ppg-net-test',
  'test-net.c',
  include_directories : pp_inc,
  dependencies : [check],
  link_with : [lib_pp_net, lib_pp_std],
  link_args : extra_args,
  install : false
)

# Testing bmp suite
ppg_bmp_test = executable('ppg-bmp-test',
  'test-bmp.c',
  include_directories : pp_inc,
  dependencies : [check],
  link_with : [lib_pp, lib_pp_std],
  link_args : extra_args,
  install : false
)

# Testing img suite
ppg_img_test = executable('ppg-img-test',
  'test-img.c',
  include_directories : pp_inc,
  dependencies : [check],
  link_with : [lib_pp, lib_pp_std],
  link_args : extra_args,
  install : false
)

# Testing for user input
ppg_user_input_test = executable('ppg-user-input-test',
  'test-user-input.c',
  include_directories : pp_inc,
  dependencies : [check],
  link_with : [lib_pp, lib_pp_std],
  link_args : extra_args,
  install : false
)

run_command('cp', 'bmps/background.bmp', 'bmps/image.bmp', test_dir)
run_command('cp',
  'imgs/background.png',
  'imgs/image.png',
  'imgs/sprit.png',
  test_dir
)

test('ppg-net-test', ppg_net_test, suite: ['net'])
test('ppg-bmp-test', ppg_bmp_test, suite: ['all'])
test('ppg-img-test', ppg_img_test, suite: ['all'])
test('ppg-user-input-test', ppg_user_input_test, suite: ['all'])
