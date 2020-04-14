//
// Example plugin: it just swaps the blue and green color component
// values for each pixel in the source image.
//

#include <stdlib.h>
#include <stdio.h>
#include "image_plugin.h"

struct Arguments {
  int n;
};

const char *get_plugin_name(void) {
  return "tile";
}

const char *get_plugin_desc(void) {
  return "tile source image in an NxN arrangement";
}

void *parse_arguments(int num_args, char *args[]) {
  if (num_args != 1) {
    return NULL;
  }
  if (atoi(args[0]) < 0) {
    return NULL;
  }
  struct Arguments *arguments = malloc(sizeof(struct Arguments));
  arguments->n = atoi(args[0]);
  return arguments;
}

struct Image *transform_image(struct Image *source, void *arg_data) {
  struct Arguments *args = arg_data;
  
  // Allocate a result Image
  struct Image *out = img_create(source->width, source->height);
  if (!out) {
    free(args);
    return NULL;
  }

  int x_base = source->width / args->n;
  int x_carry = source->width % args->n;

  int y_base = source->height / args->n;
  int y_carry = source->height % args->n;

  int x_coords[args->n + 1];
  int y_coords[args->n + 1];

  x_coords[0] = 0;
  y_coords[0] = 0;
  
  for (int i = 1; i < args->n + 1; i++) {
    if (x_carry > 0) {
      x_coords[i] = x_coords[i - 1] + x_base + 1; 
      x_carry--;
    } else {
      x_coords[i] = x_coords[i - 1] + x_base;
    }
    if (y_carry > 0) {
      y_coords[i] = y_coords[i - 1] + y_base + 1;
      y_carry--;
    } else {
      y_coords[i] = y_coords[i - 1] + y_base;
    }
  }

  for (int j = 0; j < args->n + 1; j++) {
    printf("x: %u y: %u\n", x_coords[j], y_coords[j]);
  }
  free(args);
  return out;
}
