/*
 * Chromium EFL
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */
#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_pixmap_set : public utc_blink_ewk_base
{
};

/**
 * @brief Tests if can set pixmap id.
 */
TEST_F(utc_blink_ewk_context_pixmap_set, POS_TEST)
{
  Ewk_Context* context = ewk_context_default_get();
  ASSERT_TRUE(context);

  ASSERT_EQ(EINA_TRUE, ewk_context_pixmap_set(context, 1));

  EXPECT_EQ(1, ewk_context_pixmap_get(context));
}

/**
 * @brief Tests if returns false when context is null.
 */
TEST_F(utc_blink_ewk_context_pixmap_set, NEG_TEST)
{
  ASSERT_EQ(EINA_FALSE, ewk_context_pixmap_set(NULL, 1));
}
