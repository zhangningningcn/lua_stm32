#ifndef _IO_H_
#define _IO_H_

#include "spiffs.h"

#define EOF                      -1


#define STD_IN   0
#define STD_OUT  1
#define STD_ERR  2
#define IO_NULL  3



#define SEEK_SET   SPIFFS_SEEK_SET
#define SEEK_CUR   SPIFFS_SEEK_CUR
#define SEEK_END   SPIFFS_SEEK_END
typedef struct FILE_{
    spiffs_file fd;
}FILE;

void fsinit(void);

void Lua_InPutOutPutDisable(void);
void Lua_InPutOutPutEnable(void);

FILE * mio_fopen(const char *path, const char *flags);//        SPIFFS_open(__fs, p, g, 0)
s32_t mio_fclose(FILE *fp);//         SPIFFS_close(__fs, f)
s32_t mio_fflush(FILE *fp);//         SPIFFS_fflush(__fs, f)
s32_t mio_fread ( void *buffer, size_t size, size_t count, FILE *fp) ;
s32_t mio_fwrite(const void *buffer, size_t size, size_t count, FILE *fp);
s32_t mio_fseek(FILE *fp, s32_t offs, int whence);//    SPIFFS_lseek(__fs,f,o,w)
s32_t mio_getc(FILE *fp);         //SPIFFS_getc(__fs,f)  //读取一个字节
s32_t mio_ungetc(const char c,FILE *fp);       //SPIFFS_lseek(__fs,f,-1,SPIFFS_SEEK_CUR)
char * mio_fgets(void *buffer, size_t size, FILE *fp);       //buffer, sizeof(buffer), stdin
int mio_feof(FILE *fp);
int mio_ftell(FILE *fp);
void mio_clearerr(FILE *fp);
int mio_ferror(FILE *fp);
s32_t mio_fremove(FILE *fp);
s32_t mio_remove(const char *name);
s32_t mio_rename(const char *opath,const char *npath);
FILE * mio_freopen(const char *path, const char *flags,FILE *fp);
int mio_fputs(const char *str, FILE *fp); 
s32_t mio_format(void);
s32_t mio_info(uint32_t *total,uint32_t *count);
spiffs_DIR *mio_opendir(const char *name, spiffs_DIR *d) ;
struct spiffs_dirent *mio_readdir(spiffs_DIR *d, struct spiffs_dirent *e);

int fprintf(FILE *fp, const char *str, ...);
int sprintf(char *buffer, const char *str, ...);
int snprintf(char *buffer, size_t size, const char *format, ...);

#define PRINTF_BUFFER_SIZE  128



extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


#endif
