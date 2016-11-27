//iodefine.h
#ifndef _IO_DEFINE_H_
#define _IO_DEFINE_H_

#define l_fopen(p,m)          mio_fopen(p,m)
#define l_fclose(f)           mio_fclose(f)
#define l_fflush(f)           mio_fflush(f)
#define l_fread(b, s, c, f)   mio_fread (b, s, c, f)
#define l_fwrite(b, s, c, f)  mio_fwrite(b, s, c, f)
#define l_fseek(f, o, w)      mio_fseek(f, o, w)
#define l_getc(f)             mio_getc(f)
#define l_ungetc(c,f)         mio_ungetc(c,f)
#define l_fgets(b, s, f)      mio_fgets(b, s, f)
#define l_feof(f)             mio_feof(f)
#define l_ftell(f)            mio_ftell(f)
#define l_clearerr(f)         mio_clearerr(f)
#define l_ferror(f)           mio_ferror(f)
#define l_remove(n)           mio_remove(n)
#define l_rename(o,n)         mio_rename(o,n)
#define l_freopen(p, g,f)     mio_freopen(p, g,f)
#define l_fputs(t,f)          mio_fputs(t,f)
#define l_gmtime(t,r)         rtc_gmtime(t,r)
#define l_localtime(t,r)      rtc_localtime(t,r)
#define l_time(t)             rtc_time(t)
#define l_mktime(t)           rtc_mktime(t)
#define l_difftime(t,r)       rtc_difftime(t,r)
#define l_strftime(p,s,f,t)   rtc_strftime(p, s, f, t)
#define l_lockfile(f)
#define l_unlockfile(f)

#define l_seeknum		long


#define LUA_MAXINPUT		128

#endif
