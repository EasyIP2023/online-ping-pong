#include <check.h>
#include <common.h>
#include <net.h>

START_TEST(test_create_serv) {
  bool err = false;

  struct ppg_server_t server;
  err = ppg_create_server(&server, 3001, 64, 1000);
  if (!err) {
    ppg_freeup_server(&server);
    ck_abort_msg(NULL);
  }

  ppg_freeup_server(&server);
} END_TEST;

Suite *main_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("TestServer");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_create_serv);
  suite_add_tcase(s, tc_core);

  return s;
}

int main (void) {
  int number_failed;
  SRunner *sr = NULL;

  sr = srunner_create(main_suite());
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  sr = NULL;
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
