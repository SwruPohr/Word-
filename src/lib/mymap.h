#pragma once

#include <stdint.h>



typedef struct MyMap MyMap;

enum { PROT_READ = 1, PROT_WRITE = 2 };

static inline int  mymap_map_file(MyMap* m, const char* path_utf8, int prot);
static inline void mymap_unmap(MyMap* m);

#if defined(_WIN32)
  #include <windows.h>
  #include <stdlib.h>

  typedef struct MyMap {
    void*  addr;
    size_t size;
    HANDLE hMap;
    HANDLE hFile;
  } MyMap;

  static inline wchar_t* my_utf8_to_wide_strdup(const char* s) {
    if (!s) return NULL;

    int n = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
    if (n <= 0) return NULL;

    wchar_t* w = (wchar_t*)malloc((size_t)n * sizeof(wchar_t));
    if (!w) return NULL;

    if (!MultiByteToWideChar(CP_UTF8, 0, s, -1, w, n)) {
      free(w);
      return NULL;
    }
    return w;
  }

  /* Windows version: takes UTF-8 char* path, converts internally, frees internally. */
  static inline int mymap_map_file(MyMap* m, const char* path_utf8, int prot) {
    if (!m || !path_utf8) return 0;

    wchar_t* wpath = my_utf8_to_wide_strdup(path_utf8);
    if (!wpath) return 0;

    m->addr = NULL;
    m->size = 0;
    m->hMap = NULL;
    m->hFile = INVALID_HANDLE_VALUE;

    DWORD desiredAccess = (prot & MyProt_Write) ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ;
    m->hFile = CreateFileW(wpath, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    free(wpath);
    wpath = NULL;

    if (m->hFile == INVALID_HANDLE_VALUE) return 0;

    LARGE_INTEGER li;
    if (!GetFileSizeEx(m->hFile, &li) || li.QuadPart <= 0) {
      CloseHandle(m->hFile);
      m->hFile = INVALID_HANDLE_VALUE;
      return 0;
    }

    ULONGLONG u = (ULONGLONG)li.QuadPart;
    DWORD sizeLow  = (DWORD)(u & 0xFFFFFFFFu);
    DWORD sizeHigh = (DWORD)((u >> 32) & 0xFFFFFFFFu);

    DWORD protect   = (prot & MyProt_Write) ? PAGE_READWRITE : PAGE_READONLY;
    DWORD mapAccess = (prot & MyProt_Write) ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ;

    m->hMap = CreateFileMappingW(m->hFile, NULL, protect, sizeHigh, sizeLow, NULL);
    if (!m->hMap) {
      CloseHandle(m->hFile);
      m->hFile = INVALID_HANDLE_VALUE;
      return 0;
    }

    m->addr = MapViewOfFile(m->hMap, mapAccess, 0, 0, 0 /* whole file */);
    if (!m->addr) {
      CloseHandle(m->hMap);
      m->hMap = NULL;
      CloseHandle(m->hFile);
      m->hFile = INVALID_HANDLE_VALUE;
      return 0;
    }

    m->size = (size_t)li.QuadPart;
    return 1;
  }

  static inline void mymap_unmap(MyMap* m) {
    if (!m) return;

    if (m->addr) {
      UnmapViewOfFile(m->addr);
      m->addr = NULL;
    }
    if (m->hMap) {
      CloseHandle(m->hMap);
      m->hMap = NULL;
    }
    if (m->hFile && m->hFile != INVALID_HANDLE_VALUE) {
      CloseHandle(m->hFile);
      m->hFile = INVALID_HANDLE_VALUE;
    }
    m->size = 0;
  }

#elif defined(__APPLE__) || defined(__linux__)

  #include <sys/mman.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>

  typedef struct MyMap {
    void*  addr;
    size_t size;
    int    fd;
  } MyMap;

  /* Non-Windows version: takes UTF-8 char* path directly. */
  static inline int mymap_map_file(MyMap* m, const char* path_utf8, int prot) {
    if (!m || !path_utf8) return 0;

    m->addr = NULL;
    m->size = 0;
    m->fd = -1;

    int flags = (prot & MyProt_Write) ? O_RDWR : O_RDONLY;
    m->fd = open(path_utf8, flags);
    if (m->fd < 0) return 0;

    struct stat st;
    if (fstat(m->fd, &st) != 0) {
      close(m->fd);
      m->fd = -1;
      return 0;
    }
    if (st.st_size <= 0) {
      close(m->fd);
      m->fd = -1;
      return 0;
    }

    int pmode = 0;
    if (prot & MyProt_Read)  pmode |= PROT_READ;
    if (prot & MyProt_Write) pmode |= PROT_WRITE;

    void* p = mmap(NULL, (size_t)st.st_size, pmode, MAP_SHARED, m->fd, 0);
    if (p == (void*)-1) {
      close(m->fd);
      m->fd = -1;
      return 0;
    }

    m->addr = p;
    m->size = (size_t)st.st_size;
    return 1;
  }

  static inline void mymap_unmap(MyMap* m) {
    if (!m) return;

    if (m->addr && m->size) {
      munmap(m->addr, m->size);
      m->addr = NULL;
    }
    if (m->fd >= 0) {
      close(m->fd);
      m->fd = -1;
    }
    m->size = 0;
  }

#else
  #error "Unsupported platform"
#endif
