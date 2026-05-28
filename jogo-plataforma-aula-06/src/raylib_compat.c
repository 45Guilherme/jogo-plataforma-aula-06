#ifdef __MINGW64__
int _stat64i32(const char *path, void *buffer);

int stat64i32(const char *path, void *buffer)
{
    return _stat64i32(path, buffer);
}
#endif
