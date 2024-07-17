#pragma once

#include <signal.h>

extern volatile sig_atomic_t input_buffer_is_dirty;

void set_signal_handlers(void);
