void createFile(char filename[256]);
void copyFile(char filename[256], char pathname[256]);
void moveFile(char filename[256], char pathname[256]);
void createFolder(char dirname[256]);
void get_dir_content(char *dirname, char dirArray[256][256]);
int get_dir_size(char *dirname);
int is_regular_file(const char *path);
