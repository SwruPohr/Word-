#pragma once


#define OPTION_INIT(T) {
	enum {Some, None} x;
	union {void, T} x;
} OPTION_##T;