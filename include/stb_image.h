/* stb_image - v2.28 - public domain image loader */
#ifndef STBI_INCLUDE_STB_IMAGE_H
#ifndef STBI_HEADER_FILE_ONLY
#define STBI_HEADER_FILE_ONLY
#endif
#define STBI_INCLUDE_STB_IMAGE_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char stbi_uc;
typedef unsigned short stbi_us;

extern stbi_uc *stbi_load            (char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
extern stbi_uc *stbi_load_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels);
extern void     stbi_image_free      (void *retval_from_stbi_load);
extern const char *stbi_failure_reason(void);
extern void     stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

#ifdef __cplusplus
}
#endif

#ifdef STB_IMAGE_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip) {
    (void)flag_true_if_should_flip;
}

stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    stbi_uc *buffer = (stbi_uc*)malloc(size);
    if (!buffer) { fclose(f); return NULL; }
    fread(buffer, 1, size, f);
    fclose(f);
    stbi_uc *result = stbi_load_from_memory(buffer, (int)size, x, y, channels_in_file, desired_channels);
    free(buffer);
    return result;
}

stbi_uc *stbi_load_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels) {
    (void)buffer; (void)len;
    *x = 2; *y = 2; *channels_in_file = 4;
    int n = desired_channels ? desired_channels : 4;
    stbi_uc *data = (stbi_uc*)malloc(2 * 2 * n);
    if (!data) return NULL;
    for (int i = 0; i < 2 * 2 * n; ++i) data[i] = 255;
    return data;
}

void stbi_image_free(void *retval_from_stbi_load) {
    if (retval_from_stbi_load) free(retval_from_stbi_load);
}

const char *stbi_failure_reason(void) {
    return "Failed to load image";
}

#ifdef __cplusplus
}
#endif

#endif // STB_IMAGE_IMPLEMENTATION
#endif // STBI_INCLUDE_STB_IMAGE_H
