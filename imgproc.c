#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/types.h>
#include <stddef.h>

struct Plugin {
  void *handle;
  const char *(*get_plugin_name)(void);
  const char *(*get_plugin_desc)(void);
  void *(*parse_arguments)(int num_args, char *args[]);
  struct Image *(*transform_image)(struct Image *source, void *arg_data);
  int temp;
};

int get_directory_size() {
  int file_count = 0;
  //use getenv to check for PLUGIN_DIR first
  DIR * dir = opendir("./plugins");
  struct dirent * dr;
  while ((dr = readdir(dir)) != NULL) {
    file_count++;
  }
  closedir(dir);
  return file_count;
}

void find_files(struct Plugin plugins[], size_t len) {
  //use getenv to check for PLUGIN_DIR first
  char * dir_name = "./plugins";
  DIR * dir = opendir(dir_name);
  //error if null dir_name
  struct dirent * dr;
  size_t index = 0;
  while ((dr = readdir(dir)) != NULL) {
    char * file = dr->d_name;
    char fullpath[1000];
    snprintf(fullpath, 1000, "%s/%s", dir_name, dr->d_name);
    fullpath[999] = '\0';
    puts(fullpath);
    void * handle = dlopen(fullpath, RTLD_LAZY);
    plugins[index].temp = index;
    index++;
  }
  //puts(fullpath);
  closedir(dir);
}

int main (int argc, char *argv[1]) {
  size_t dir_size = get_directory_size();
  struct Plugin plugins[dir_size - 2];
  //change this to use normal sizes lol???
  //find_files(plugins, sizeof(plugins) / sizeof(*plugins));
  find_files(plugins, dir_size - 2);

  for (size_t i = 0; i < dir_size; i++) {
    printf("%zu: %d\n", i, plugins[i].temp);
  }
  
  if (strcmp(argv[1], "list") == 0) {
    //list();
  } else if (strcmp(argv[1], "exec") == 0) {
    //findFiles();
  } else {
    return 1; //bad param somewhere
  }
  return 0;
}

