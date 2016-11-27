
#include <string.h>
#include <stdarg.h>
#include "spiffs.h"
#include "app_usart.h"
#include "io.h"


static spiffs __fs;

static FILE stdfile[] = {
    {STD_IN},
    {STD_OUT},
    {STD_ERR},
};
FILE *stdin  = &stdfile[0];
FILE *stdout = &stdfile[1];
FILE *stderr = &stdfile[2];


void fsinit(void) {
    SPIFlashInit(&__fs); 
}

static spiffs_mode str2spifs_mode (const char *mode) {
    spiffs_mode sfsmod;
    // SPIFFS_O_APPEND, SPIFFS_O_TRUNC, SPIFFS_O_CREAT, SPIFFS_O_RDONLY,
    // SPIFFS_O_WRONLY, SPIFFS_O_RDWR, SPIFFS_O_DIRECT, SPIFFS_O_EXCL
    sfsmod = 0;
    if(mode == NULL || strlen(mode) == 0) {
        return SPIFFS_O_RDONLY;
    }
    if(strchr(mode,'r') != NULL) {
        sfsmod |= SPIFFS_O_RDONLY;
    }
    if(strchr(mode,'w') != NULL) {
        sfsmod |= SPIFFS_O_WRONLY;
    }
    if(strchr(mode,'a') != NULL) {
        sfsmod |= SPIFFS_O_APPEND;
        sfsmod |= SPIFFS_O_WRONLY;
    }
    if(strchr(mode,'+') != NULL) {
        sfsmod |= SPIFFS_O_CREAT;
    }
    return sfsmod;
}
FILE * mio_fopen(const char *path, const char *flagstr) {
    FILE *f = (FILE *)malloc(sizeof(FILE));
    if(f == NULL) {return NULL;}
    spiffs_flags flags = str2spifs_mode(flagstr);
    f->fd = SPIFFS_open(&__fs, path, flags, 0);
    if(f->fd < 0) {
        free(f);
        f = NULL;
    }
    return f;
}
FILE * mio_freopen(const char *path, const char *flagstr,FILE *fp) {
    spiffs_file fd = fp->fd;
    if(fd >= SPIFFS_FILEHDL_OFFSET) {
        SPIFFS_close(&__fs, fd);
        spiffs_flags flags = str2spifs_mode(flagstr);
        fp->fd = SPIFFS_open(&__fs, path, flags, 0);
        if(fp->fd < 0) {
            free(fp);
            fp = NULL;
        }
        return fp;
    }
    else {
        USART_Lua_SendS("freopen not support stdin\\out\\err stream",'\n');
    }
    return NULL;
}
s32_t mio_fclose(FILE * fp) {
    spiffs_file fd = fp->fd;
    if(fd >= SPIFFS_FILEHDL_OFFSET) {
        free(fp);
        return SPIFFS_close(&__fs, fd);
    }
    return 0;
}
s32_t mio_fflush(FILE *fp) {
    spiffs_file fd = fp->fd;
    if(fd >= SPIFFS_FILEHDL_OFFSET) {
        return SPIFFS_fflush(&__fs, fd);
    }
    return 0;
};
s32_t mio_fread( void *buffer, size_t size, size_t count, FILE *fp) {
    int len = size*count;
    spiffs_file fd = fp->fd;
    if(fd >= SPIFFS_FILEHDL_OFFSET) {
        return SPIFFS_read(&__fs,fd,buffer,len);
    }
    else if(fd == STD_IN) {
        return USART_Lua_Recive(buffer,len);
    }
    return 0;
};

s32_t mio_fwrite(const void *buffer, size_t size, size_t count, FILE *fp) {
    int len = size*count;
    spiffs_file fd = fp->fd;
    if(fd >= SPIFFS_FILEHDL_OFFSET) {
        return SPIFFS_write(&__fs,fd,(void *)buffer,len);
    }
    else if(fd == STD_OUT || fd == STD_ERR) {
        return USART_Lua_Send(buffer,len);
    }
    return 0;
}
s32_t mio_fseek(FILE *fp, s32_t offs, int whence) {
    spiffs_file fd = fp->fd;
    if(fd >= SPIFFS_FILEHDL_OFFSET) {
        return SPIFFS_lseek(&__fs,fd,offs,whence);
    }
    return 0;
}
s32_t mio_getc(FILE *fp) {
    spiffs_file fd = fp->fd;
    if(fd >= SPIFFS_FILEHDL_OFFSET) {
        u8_t buf;
        int len = SPIFFS_read(&__fs, fd, &buf, 1);
        if(len <= 0) {
            return EOF;
        }
        return buf & 0x00FF;
    }
    else if(fd == STD_IN) {
        return USART_Lua_Getchar();
    }
    return EOF;
}         //SPIFFS_getc(&__fs,f)  //读取一个字节
s32_t mio_ungetc(const char c,FILE *fp) {
    spiffs_file fd = fp->fd;
    if(fd >= SPIFFS_FILEHDL_OFFSET) {
        SPIFFS_lseek(&__fs,fd,-1,SPIFFS_SEEK_CUR);
    }
    else if(fd == STD_IN) {
        USART_Lua_UnGetchar(c);
    }
    return 0;
}
char * mio_fgets(void *buffer, size_t size, FILE *fp) {
    char * pstr = (char *)buffer;
    spiffs_file fd = fp->fd;
    if(fd >= SPIFFS_FILEHDL_OFFSET) {
        char buf;
        buf = 0;
        while(buf != '\n') {
            int len = SPIFFS_read(&__fs, fd, &buf, 1);
            if(len <= 0) {
                break;
            }
            if(buf == '\r') {
                continue;
            }
            if(buf == '\n' || buf == '\0') {
                *pstr = '\0';
                break;
            }
            else {
                *pstr = buf;
                pstr++;
            }
        }
    }
    else if(fd == STD_IN) {
        int len = USART_Lua_ReciveS(pstr,size);
        if(len < 0) {
            return NULL;
        }
        pstr += len;
    }
    if(pstr == buffer) {
        return NULL;
    }
    return (char *)buffer;
}
int mio_fputs(const char *str, FILE *fp) {
//    char * pstr = str;
    int sendlen = 0;
    spiffs_file fd = fp->fd;
    if(fd >= SPIFFS_FILEHDL_OFFSET) {
        int len = strlen(str);
        sendlen = SPIFFS_write(&__fs,fd,(void *)str,len);
    }
    else if(fd == STD_OUT || fd == STD_ERR) {
        sendlen += USART_Lua_SendS(str,'\0');
    }
    return sendlen;
}
int mio_feof(FILE *fp) {
    spiffs_file fd = fp->fd;
    if(fd >= SPIFFS_FILEHDL_OFFSET) {
        return SPIFFS_eof(&__fs,fd);
    }
    else if(fd == STD_IN) {
        return USART_Lua_RecBufferEOF();
    }
    return 1;
}
int mio_ftell(FILE *fp)
{
    spiffs_file fd = fp->fd;
    if(fd >= SPIFFS_FILEHDL_OFFSET) {
        return SPIFFS_tell(&__fs,fd);
    }
    else if(fd == STD_IN){
        return USART_Lua_RecTell();
    }
    return 0;
}

void mio_clearerr(FILE *fp)
{
    //void SPIFFS_clearerr(spiffs *fs);
    SPIFFS_clearerr(&__fs);
}
int mio_ferror(FILE *fp)
{
    //s32_t SPIFFS_errno(spiffs *fs);
    return SPIFFS_errno(&__fs);
}
int mio_fprintf(FILE *fp, const char *str, ...)
{
    int len;
    va_list ap;
    spiffs_file fd = fp->fd;
    char *p_buffer = malloc(PRINTF_BUFFER_SIZE);
    if(p_buffer == NULL) {
        return 0;
    }
    va_start(ap,str);
    len = sprintf(p_buffer,str,ap);
    if(len >= PRINTF_BUFFER_SIZE) {
        USART_Lua_SendS("fprintf buffer size Error",'\n');
        //ToDo
        //Delayms(500);
        //Reset
        free(p_buffer);
        return 0;
    }
    if(fd >= SPIFFS_FILEHDL_OFFSET) {
        SPIFFS_write(&__fs,fd,p_buffer,len);
        //return len;
    }
    else if(fd == STD_IN){
        USART_Lua_SendS(str,'\0');
        //USART_Lua_SendS("\r\n");
    }
    free(p_buffer);
    return len;
}

s32_t mio_remove(const char *name) {
    return SPIFFS_remove(&__fs, name);
}

s32_t mio_fremove(FILE *fp) {
    spiffs_file fd = fp->fd;
    return SPIFFS_fremove(&__fs, fd);
}


s32_t mio_rename(const char *opath,const char *npath) {
    return SPIFFS_rename(&__fs, opath,npath);
}

s32_t mio_format(void) {
    SPIFFS_unmount(&__fs);
    int res = SPIFFS_format(&__fs);
    SPIFlashMount(&__fs);
    return res;
}
    
s32_t mio_info(uint32_t *total,uint32_t *count) {
    return SPIFFS_info(&__fs,total,count);
}
spiffs_DIR * mio_opendir(const char *name, spiffs_DIR *d) {
    return SPIFFS_opendir(&__fs,name,d);
}
struct spiffs_dirent *mio_readdir(spiffs_DIR *d, struct spiffs_dirent *e) {
    return SPIFFS_readdir(d,e);
}
void Lua_InPutOutPutDisable(void) {
    stdfile[0].fd = IO_NULL;
    stdfile[1].fd = IO_NULL;
    stdfile[2].fd = IO_NULL;
}

void Lua_InPutOutPutEnable(void) {
    stdfile[0].fd = STD_IN;
    stdfile[1].fd = STD_OUT;
    stdfile[2].fd = STD_ERR;
}

