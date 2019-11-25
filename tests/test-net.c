#include <check.h>
#include <common.h>
#include <net.h>
#include <tpool.h>

/* 3 Threads 100 work items */
#define NUM_THREADS 3
#define NUM_ITEMS 100

void worker(void *arg) {
  int *val = (int *) arg;
  int old = *val;

  *val += 1000;
  ppg_log_me(PPG_WARNING, "tid=%p, old=%d, val=%d", pthread_self(), old, *val);

  if (*val%2) usleep(100000);
}

START_TEST(test_create_serv) {
  ppg_server_t *server = ppg_create_server(3001, 64, 1000);
  if (!server) {
    ppg_freeup_server(server);
    ck_abort_msg(NULL);
  }

  ppg_freeup_server(server);
} END_TEST;

START_TEST(test_thread_pool) {
  tpool_t *tp = NULL;
  int vals[NUM_ITEMS];
  uint32_t i = 0;

  tp = ppg_tpool_create(NUM_THREADS);
  if (!tp) ck_abort_msg(NULL);

  for (i=0; i < NUM_ITEMS; i++) {
    vals[i] = i;
    if (!ppg_tpool_add_work(tp, worker, vals+i)) {
      ppg_log_me(PPG_DANGER, "[x] ppg_tpool_add_work failed");
      ck_abort_msg(NULL);
    }
  }

  ppg_tpool_wait(tp);

  for (i=0; i < NUM_ITEMS; i++)
    ppg_log_me(PPG_INFO, "%d", vals[i]);

  ppg_freeup_tpool(tp);
} END_TEST;

Suite *main_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("TestServer");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_create_serv);
  tcase_add_test(tc_core, test_thread_pool);
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
