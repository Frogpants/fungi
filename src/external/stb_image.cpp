// src/thirdparty/stb_image.cpp
#define STB_IMAGE_IMPLEMENTATION
#if __has_include(<stb_image.h>)
#include <stb_image.h>
#elif __has_include(<stb/stb_image.h>)
#include <stb/stb_image.h>
#else
#error "stb_image header not found"
#endif
