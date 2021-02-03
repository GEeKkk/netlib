#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "netlib/base/Timestamp.h"
#include <functional>

using TimerCallback = std::function<void()>;

#endif // CALLBACKS_H