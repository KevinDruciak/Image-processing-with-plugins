//
// Example plugin: it just swaps the blue and green color component
// values for each pixel in the source image.
//

#include <stdlib.h>
#include <stdio.h>
#include "image_plugin.h"

struct Arguments {
  double expose;
};

const char *get_plugin_name(void) {
  return "expose";
}

const char *get_plugin_desc(void) {
  return "adjust the intensity of all pixels";
}

void *parse_arguments(int num_args, char *args[]) {
  if (num_args != 1) {
    return NULL;
  }
  if (atof(args[0]) < 0) {
    return NULL;
  }
  struct Arguments *arguments = malloc(sizeof(struct Arguments));
  arguments->expose = atof(args[0]);
  return arguments;
}

// Helper function to swap the blue and green color component values.
static uint32_t expose(uint32_t pix, struct Arguments *args) {
  uint8_t rgba[4];
  img_unpack_pixel(pix, &rgba[0], &rgba[1], &rgba[2], &rgba[3]);
  for (size_t i = 0; i < 4; i++) {
    uint16_t temp = rgba[i] * args->expose;
    if (temp > 255) {
      rgba[i] = 255;
    } else {
      rgba[i] = (uint8_t) temp;
    }
  } 
  return img_pack_pixel(rgba[0], rgba[1], rgba[2], rgba[3]);
}

struct Image *transform_image(struct Image *source, void *arg_data) {
  struct Arguments *args = arg_data;
  
  // Allocate a result Image
  struct Image *out = img_create(source->width, source->height);
  if (!out) {
    free(args);
    return NULL;
  }
  
  unsigned num_pixels = source->width * source->height;
  for (unsigned i = 0; i < num_pixels; i++) {
    out->data[i] = expose(source->data[i], args);
  }
  
  free(args);
  return out;
}
