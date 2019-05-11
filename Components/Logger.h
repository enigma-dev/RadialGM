#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>
#include <QtGlobal>

class LogAndReturn {
  QDebug logger;

 public:
  LogAndReturn(QtMsgType msgType) : logger(msgType) {}
  LogAndReturn(const QDebug& log) : logger(log) {}

  template <typename T>
  LogAndReturn& operator<<(T t) {
    logger << t;
    return *this;
  }
  struct Void {
    void operator,(const LogAndReturn& /*log*/) {}
  };
};
template <typename T>
T operator,(T x, const LogAndReturn&) {
  return x;
}

// check and log the condition, return value when failed
#define R_EXPECT(cond, ret) \
  if (cond) {               \
  } else                    \
    return ret, LogAndReturn(qWarning()) << "Check `" #cond "` failed."

// check and log the condition, return void when failed
#define R_EXPECT_V(cond) \
  if (cond) {            \
  } else                 \
    return LogAndReturn::Void(), LogAndReturn(qWarning()) << "Check `" #cond "` failed."

// check and log the condition, but continue on failure
#define R_ASSESS(cond) \
  if (cond) {          \
  } else               \
    LogAndReturn(qWarning()) << "Check `" #cond "` failed."

#endif  // LOGGER_H
