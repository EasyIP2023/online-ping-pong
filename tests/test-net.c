#include <check.h>
#include <common.h>
#include <net.h>

/* 3 Threads 100 work items */
#define NUM_THREADS 3
#define NUM_ITEMS 100

void worker(void *arg1, void *arg2) {
  ppg_server_t *serv = (ppg_server_t *) arg1;
  uint32_t *val = (uint32_t *) arg2;
  uint32_t old = *val;

  *val += 1000;
  ppg_log_me(PPG_WARNING, "tid=%p, old=%d, val=%d, serv=%p", pthread_self(), old, *val, serv);

  if (*val%2) usleep(100000);
}

START_TEST(test_create_serv) {
  ppg_server_t *server = ppg_create_server(3001, 64, 1000);
  if (!server) {
    ppg_freeup_server(server);
    ck_abort_msg(NULL);
  }

  ppg_freeup_server(server);
  ppg_log_me(PPG_DANGER, "Successfully freed memory");
} END_TEST;

START_TEST(test_thread_pool) {
  tpool_t *tp = NULL;
  int vals[NUM_ITEMS];
  uint32_t i = 0;

  tp = ppg_tpool_create(NUM_THREADS);
  if (!tp) ck_abort_msg(NULL);

  for (i=0; i < NUM_ITEMS; i++) {
    vals[i] = i;
    if (!ppg_tpool_add_work(tp, worker, NULL, vals+i)) {
      ppg_freeup_tpool(tp);
      ppg_log_me(PPG_DANGER, "[x] ppg_tpool_add_work failed");
      ck_abort_msg(NULL);
    }
  }

  ppg_tpool_wait(tp);

  for (i=0; i < NUM_ITEMS; i++)
    ppg_log_me(PPG_INFO, "%d", vals[i]);

  ppg_freeup_tpool(tp);
} END_TEST;

/* This will leave a bunch of client file descriptors open */
START_TEST(test_create_multiple_games) {
  ppg_server_t *server = ppg_create_server(5001, 64, 1000);
  if (!server) {
    ppg_freeup_server(server);
    ck_abort_msg(NULL);
  }

  /* Number of threads is equal to the number of CPU cores I have */
  tpool_t *tp = ppg_tpool_create(3);
  if (!tp) {
    ppg_freeup_server(server);
    ck_abort_msg(NULL);
  }

  switch (fork()) {
  case -1:
    ppg_log_me(PPG_DANGER, "[x] fork failed");
    ck_abort_msg(NULL);
  case 0:
    for (uint32_t i = 0; i < 30; i++)
      ppg_connect_client("127.0.0.1", 5001);
    break;
  default:
    while(ppg_epoll_server(server, tp)) {
      ppg_tpool_wait(tp);
    }
    break;
  }

  ppg_freeup_tpool(tp);
  ppg_freeup_server(server);
} END_TEST;

Suite *main_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("TestServer");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_create_serv);
  tcase_add_test(tc_core, test_thread_pool);

  /* This will leave a bunch of client file descriptors open */
  tcase_add_test(tc_core, test_create_multiple_games);
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
