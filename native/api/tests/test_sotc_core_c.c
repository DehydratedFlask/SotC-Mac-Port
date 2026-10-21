/* Compiled as C99: proves sotc_core.h is a plain C header and the whole flow links from C. */
#include <stdio.h>
#include <string.h>

#include "sotc_core.h"

static int fail(const char* what) {
  fprintf(stderr, "FAIL %s\n", what);
  return 1;
}

int main(void) {
  sotc_core* core = NULL;
  sotc_validation_result r;
  sotc_status st;
  char msg[256];

  if (sotc_core_create(SOTC_CORE_ABI_VERSION, &core) != SOTC_OK) return fail("create");
  memset(&r, 0, sizeof r);
  r.struct_size = sizeof r;
  if (sotc_core_validate(core, "/nonexistent/sotc_c_test", &r) != SOTC_OK) return fail("validate");
  if (r.assets_ready != 0 || r.engine_ready != 0 || r.start_would_succeed != 0) return fail("result");
  if (sotc_core_start(core) != SOTC_ERR_ASSETS_NOT_READY) return fail("start-not-ready");
  memset(&st, 0, sizeof st);
  st.struct_size = sizeof st;
  if (sotc_core_status(core, &st) != SOTC_OK || st.state == SOTC_STATE_RUNNING) return fail("status");
  if (sotc_core_error_message(SOTC_ERR_ENGINE_NOT_IMPLEMENTED, msg, sizeof msg) == 0) return fail("message");
  sotc_core_destroy(core);
  puts("sotc_core_c: ok");
  return 0;
}
