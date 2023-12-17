//
// Created by rplaz on 2023-12-17.
//

#ifndef ASSERT_H
#define ASSERT_H


namespace NycaTech {

inline const char* ErrorMessage;

#define AssertReturnFalse(cond, msg) \
  if (!cond) {                       \
    ErrorMessage = msg;              \
    return false;                    \
  }

#define AssertReturnNull(cond, msg) \
  if (!cond) {                      \
    ErrorMessage = msg;             \
    return nullptr;                 \
  }

#ifndef RELEASE

#define Assert(cond, msg)    \
  if (!cond) {               \
    throw RuntimeError(msg); \
  }

#else

#define Assert(cond, msg) cond

#endif

};  // namespace NycaTech

#endif  // ASSERT_H
