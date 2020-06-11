#include <string.h>
#include <stdio.h>


#define TRACE (*(unsigned char *)0x40000000)
extern "C" {
    
int _read(int file, char* ptr, int len) {
    return 0;
}

int _open(int fd) {
    return 0;
}

int _close(int fd){
    return 0;
}

int _fstat_r(int fd) {
    return 0;
}

int _lseek_r(struct _reent *ptr, FILE *fp, long offset, int whence){
    return 0;
}

int _isatty_r(struct _reent *ptr, int fd) {
    return 0;
}


int _write(int file, const char *ptr, int len) {
  int x;

  for (x = 0; x < len; x++) {
    TRACE =  *ptr++;
  }

  return (len);
}

int _getpid(void) {
  return 0;
}

void _kill(int pid) { 
    return;
}

}
