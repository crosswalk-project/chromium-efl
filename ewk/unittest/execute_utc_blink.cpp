#include "gtest/gtest.h"

#include <glib-object.h>
#include <Elementary.h>

#include "ewk_main.h"

int main(int argc, char* argv[])
{
  /* 1. Initialization of the EFL (and other) libraries */
  g_type_init();
  ewk_init();
  elm_init(0, NULL);

  elm_config_preferred_engine_set("opengl_x11");

  testing::InitGoogleTest(&argc, argv);
  int retval = RUN_ALL_TESTS();

  /* 2. Closing whole EWK */
  ewk_shutdown();
  return retval;
}

