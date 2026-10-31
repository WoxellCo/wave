#ifndef WAVE_TARGET_MSVC_H
#define WAVE_TARGET_MSVC_H

#define strncpy(dest, src, n) strncpy_s((dest), (n) + 1, (src), (n));

#endif