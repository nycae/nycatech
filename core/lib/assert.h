//
// Created by rplaz on 2023-12-17.
//

#ifndef ASSERT_H
#define ASSERT_H

namespace NycaTech {

inline const char* ErrorMessage;

#ifdef DEBUG

#define AssertReturnFalse(cond, msg) \
  if (!(cond)) {                     \
    ErrorMessage = msg;              \
    return false;                    \
  }

#define AssertReturnNull(cond, msg) \
  if (!(cond)) {                    \
    ErrorMessage = msg;             \
    return nullptr;                 \
  }

#define Assert(cond, msg)    \
  if (!(cond)) {             \
    throw RuntimeError(msg); \
  }

#define AssertVK(cond, msg)  \
  if (cond != VK_SUCCESS) {  \
    throw RuntimeError(msg); \
  }

#define AssertVKReturnNull(cond, msg) \
  if (cond != VK_SUCCESS) {           \
    ErrorMessage = msg;               \
    return nullptr;                   \
  }

#define AssertVKReturnFalse(cond, msg) \
  if (cond != VK_SUCCESS) {            \
    ErrorMessage = msg;                \
    return false;                      \
  }

#else
#define AssertReturnFalse(cond, msg) cond;
#define AssertReturnNull(cond, msg) cond;
#define Assert(cond, msg) cond;
#define AssertVKReturnNull(cond, msg) cond;
#define AssertVKReturnFalse(cond, msg) cond;
#endif

};  // namespace NycaTech

#endif  // ASSERT_H
