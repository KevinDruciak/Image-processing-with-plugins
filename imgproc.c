#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include "image.h"

struct Plugin {
  void *handle;
  const char *(*get_plugin_name)(void);
  const char *(*get_plugin_desc)(void);
  void *(*parse_arguments)(int num_args, char *args[]);
  struct Image *(*transform_image)(struct Image *source, void *arg_data);
};

int get_directory_size() {
  int file_count = 0;
  char *path = getenv("PLUGIN_DIR");
  if (path == NULL) {
    path = "./plugins";
  }
  DIR * dir;
  if ((dir = opendir(path)) == NULL) {
    if (closedir(dir) != 0) {
      printf("Error: unable to close directory");
      exit(1);
    }
    printf("Error: cannot open directory");
    exit(1);
  }
  struct dirent * dr;
  while ((dr = readdir(dir)) != NULL) {
    file_count++;
  }
  if (closedir(dir) != 0) {
    printf("Error: unable to close directory");
    exit(1);
  }
  return file_count - 2;
}

bool validate_file(char *file) {
  int file_len = strlen(file);
  char *temp = strstr(file, ".so");
  if (strcmp(file, ".") == 0 || strcmp(file, "..") == 0 || temp - file != file_len - 3) {
    return false;
  }
  return true;
}

void list() {
  printf("Loaded %d plugin(s)\n", get_directory_size());
  char *path = getenv("PLUGIN_DIR");
  if (path == NULL) {
    path = "./plugins";
  }
  DIR * dir;
  if ((dir = opendir(path)) == NULL) {
    closedir(dir);
    printf("Error: cannot open directory");
    exit(1);
  }
  struct dirent * dr;
  while ((dr = readdir(dir)) != NULL) {
    char * file = dr->d_name;
    if (validate_file(file)) {
      char fullpath[1000];
      snprintf(fullpath, 1000, "%s/%s", path, dr->d_name);
      fullpath[999] = '\0';
      struct Plugin plugin;
      plugin.handle = dlopen(fullpath, RTLD_LAZY);
      *(void **) &(plugin.get_plugin_name) = dlsym(plugin.handle, "get_plugin_name");
      *(void **) &(plugin.get_plugin_desc) = dlsym(plugin.handle, "get_plugin_desc");
      printf("%s: %s\n", plugin.get_plugin_name(), plugin.get_plugin_desc());
      dlclose(plugin.handle);
    }
  }
  closedir(dir);
}

void exec(int argc, char *argv[]) {
  char *path = getenv("PLUGIN_DIR");
  if (path == NULL) {
    path = "./plugins";
  }
  DIR * dir;
  if ((dir = opendir(path)) == NULL) {
    closedir(dir);
    printf("Error: cannot open directory");
    exit(1);
  }
  struct dirent * dr;
  while ((dr = readdir(dir)) != NULL) {
    if (validate_file(dr->d_name)) {
      char fullpath[1000];
      snprintf(fullpath, 1000, "%s/%s", path, dr->d_name);
      fullpath[999] = '\0';
      struct Plugin plugin;
      plugin.handle = dlopen(fullpath, RTLD_LAZY);
      *(void **) &(plugin.get_plugin_name) = dlsym(plugin.handle, "get_plugin_name");
      if (strcmp(plugin.get_plugin_name(), argv[2]) == 0) {
	*(void **) &(plugin.parse_arguments) = dlsym(plugin.handle, "parse_arguments");
	int index = 5;
	char *args[1000];
	while (argv[index] != NULL) {
	  args[index - 5] = argv[index];
	  index++;
	}
	*(void **) &(plugin.transform_image) = dlsym(plugin.handle, "transform_image");
	struct Image *source = img_read_png(argv[3]);
	if (source == NULL) {
	  printf("Error: unable to load image");
	  exit(1);
	}
	struct Image *out = plugin.transform_image(source, plugin.parse_arguments(index - 4, args));
	if (out == NULL) {
	  printf("Error: unable to output image");
	  exit(1);
	}
	if (img_write_png(out, argv[4]) == 0) {
	  printf("Error: unable to write image to file");
	  exit(1);
	}
	img_destroy(source);
	img_destroy(out);
	dlclose(plugin.handle);
	break;
      }
      dlclose(plugin.handle);
    }
  }
  closedir(dir);
}


int main (int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: imgproc <command> [<command args...>]\nCommands are:\n \tlist\n \texec <plugin> <input img> <output img> [<plugin args...>]\n");
    exit(1);
  }
  if (strcmp(argv[1], "list") == 0) {
    list();
  } else if (strcmp(argv[1], "exec") == 0) {
    if (argc < 5) {
      printf("Usage: imgproc <command> [<command args...>]\nCommands are:\n \tlist\n \texec <plugin> <input img> <output img> [<plugin args...>]\n");
      exit(1);
    }
    exec(argc, argv);
  } else {
    printf("Usage: imgproc <command> [<command args...>]\nCommands are:\n \tlist\n \texec <plugin> <input img> <output img> [<plugin args...>]\n");
    exit(1);
  }
  return 0;
}

