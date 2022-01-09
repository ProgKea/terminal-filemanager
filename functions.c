#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include "functions.h"

void createFile(char filename[256])
{
  FILE *fp = fopen(filename, "w");

  if (fp == NULL) {
    printf("fopen(filename, \"w\") failed.\n");
    exit(0);
  }

  fclose(fp);
}

void copyFile(char filename[256], char pathname[256])
{
  int ch;
  FILE *fp1, *fp2;

  fp1 = fopen(filename, "r");
  fp2 = fopen(pathname, "w");

  while ((ch = getc(fp1)) != EOF) {
    fputc(ch, fp2);
  }

  fclose(fp1);
  fclose(fp2);
}

void createFolder(char dirname[256])
{
  mkdir(dirname, 0777);
}

void moveFile(char filename[256], char pathname[256])
{
  int ch;
  FILE *fp1, *fp2;

  fp1 = fopen(filename, "r");
  fp2 = fopen(pathname, "w");

  while ((ch = getc(fp1)) != EOF) {
    fputc(ch, fp2);
  }

  fclose(fp1);
  fclose(fp2);

  remove(filename);
}

void get_dir_content(char *dirname, char dirArray[256][256])
{
  DIR *dir;
  struct dirent *ent;
  int index = 0;
  if ((dir = opendir (dirname)) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      if (!(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)) {
        strcpy(dirArray[index], ent->d_name);
        index++;
      }
    }
    closedir (dir);
  }
}

int get_dir_size(char *dirname)
{
  DIR *dir;
  struct dirent *ent;
  int count = 0;
  if ((dir = opendir (dirname)) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      if (!(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)) {
      count++;
      }
    }
    closedir (dir);
  }
  return count;
}

int is_regular_file(const char *path)
{
  struct stat path_stat;
  stat(path, &path_stat);
  return S_ISREG(path_stat.st_mode);
}
