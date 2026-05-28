int _stat64i32(const char *path, void *buf);  
int stat64i32(const char *path, void *buf){ return _stat64i32(path, buf); }  
