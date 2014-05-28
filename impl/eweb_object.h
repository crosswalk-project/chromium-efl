/*
   Copyright (C) 2014 Samsung Electronics

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

#ifndef EWEB_OBJECT_PRIVATE
#define EWEB_OBJECT_PRIVATE

#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include <Eina.h>

class EWebObject : public base::RefCounted<EWebObject> {
 public:
  virtual const char* InstanceClassName() const = 0;
 protected:
  virtual ~EWebObject() { }
 private:
  friend class RefCounted<EWebObject>;
};

template <class T>
inline bool ewk_object_is_of_type(const EWebObject* object) {
  return (reinterpret_cast<T>(0)->className() == object->InstanceClassName());
}

template <class T>
inline bool ewk_object_cast_check(const EWebObject* object) {
  EINA_SAFETY_ON_NULL_RETURN_VAL(object, false);

  if (!ewk_object_is_of_type<T>(object)) {
      EINA_LOG_CRIT("attempt to convert object of type %s to type %s",
          object->InstanceClassName(), reinterpret_cast<T>(0)->className());
      NOTREACHED();
      return false;
  }

  return true;
}

template <class T>
inline const T ewk_object_cast(const EWebObject* object) {
  return ewk_object_cast_check<T>(object) ? static_cast<T>(object) : 0;
}

template <class T>
inline T ewk_object_cast(EWebObject* object) {
  return ewk_object_cast_check<T>(object) ? static_cast<T>(object) : 0;
}

#define EWK_OBJ_GET_IMPL_OR_RETURN(ImplClass, object, impl, ...)         \
  ImplClass* impl = ewk_object_cast<ImplClass*>(object);                 \
  if (!impl)                                                             \
      return __VA_ARGS__

#define EWK_OBJECT_DECLARE(_className)                                   \
static const char* className()                                           \
{                                                                        \
  static const char* name = #_className;                                 \
  return name;                                                           \
}                                                                        \
virtual const char* InstanceClassName() const                            \
{                                                                        \
  return className();                                                    \
}                                                                        \
typedef int ForceASemicolonAfterThisMacro

#endif
