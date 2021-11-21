#pragma once

#define ARRAY_SIZE(a)           (sizeof(a)/sizeof(a[0]))
#define INCREMENT_WRAP(x, max)  (x) = (((x) + 1) % (max))