/*
   Copyright (C) 2013 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef ewk_policy_decision_h
#define ewk_policy_decision_h

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum   Ewk_Policy_Decision_Type
 * @brief   Provides option to policy decision type.
 */
enum _Ewk_Policy_Decision_Type {
    EWK_POLICY_DECISION_USE,
    EWK_POLICY_DECISION_DOWNLOAD,
    EWK_POLICY_DECISION_IGNORE
};
typedef enum _Ewk_Policy_Decision_Type Ewk_Policy_Decision_Type;

typedef struct _Ewk_Policy_Decision Ewk_Policy_Decision;

enum _Ewk_Policy_Navigation_Type {
  EWK_POLICY_NAVIGATION_TYPE_LINK_CLICKED = 0,
  EWK_POLICY_NAVIGATION_TYPE_FORM_SUBMITTED = 1,
  EWK_POLICY_NAVIGATION_TYPE_BACK_FORWARD = 2,
  EWK_POLICY_NAVIGATION_TYPE_RELOAD = 3,
  EWK_POLICY_NAVIGATION_TYPE_FORM_RESUBMITTED = 4,
  EWK_POLICY_NAVIGATION_TYPE_OTHER = 5
};
typedef enum _Ewk_Policy_Navigation_Type Ewk_Policy_Navigation_Type;

#ifdef __cplusplus
}
#endif
#endif // ewk_policy_decision_h
