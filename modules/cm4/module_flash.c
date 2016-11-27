#include "stm32f4xx.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "modules.h"
#include "module_comm.h"
#include "spiffs.h"
#include "xmodem.h"
#include <stdlib.h>
#include <string.h>

//char *itoa(int value, char *str, int base);
#define PC2MCU  0
#define MCU2PC  1

s32_t mio_format(void);
s32_t mio_info(uint32_t *count,uint32_t *total);
//Lua: format()
static int lflash_format( lua_State* L ) {
    if (mio_format() != SPIFFS_OK) {
        return luaL_error( L, "format error." );
    }
    return 0;
}

//Lua: info()
static int lflash_info( lua_State* L ) {
    uint32_t used,total;
    char temp[20];
    mio_info(&total,&used);
    lua_writestring("Total:",1);
	  sprintf(temp,"%d",total);
    //itoa(total,temp,10);
    lua_writestring(temp,1);
    lua_writestring("\r\n",1);
    lua_writestring("Used:",1);
	  sprintf(temp,"%d",used);
    //itoa(used,temp,10);
    lua_writestring(temp,1);
    lua_writestring("\r\n",1);
    return 0;
}

//Lua: ls()
static int lflash_ls( lua_State* L ) {
    spiffs_DIR d;
    char temp[20];
    int len;
  //struct spiffs_dirent e;
  struct spiffs_dirent *pe = malloc(sizeof(struct spiffs_dirent));
  
    mio_opendir("/", &d);
    //SPIFFS_opendir(FS, "/", &d);
    while ((pe = mio_readdir(&d, pe))) {
        //printf("%s [%04x] size:%i\n", pe->name, pe->obj_id, pe->size);
        switch(pe->type) {
            case SPIFFS_TYPE_FILE:
                lua_writestring("- ",1);
                break;
            case SPIFFS_TYPE_DIR:
                lua_writestring("d ",1);
                break;
            case SPIFFS_TYPE_HARD_LINK:
                lua_writestring("h ",1);
                break;
            case SPIFFS_TYPE_SOFT_LINK:
                lua_writestring("s ",1);
                break;
        }
        lua_writestring((char *)pe->name,1);
        len = strlen((char *)pe->name);
        if(len < 10) {
            len = 10-len;
            memset(temp,' ',len);
            temp[len] = 0;
            lua_writestring(temp,1);
        }
        if(pe->type == SPIFFS_TYPE_FILE) {
						sprintf(temp,"%d",pe->size);
            //itoa(pe->size,temp,10);
            lua_writestring(temp,1);
        }
        lua_writestring("\r\n",1);
    }
    
    free(pe);
    return 0;
}
//Lua: rm("file")
static int lflash_rm( lua_State* L ) {
    const char *fname;
    size_t len;
    fname = luaL_checklstring(L, 1, &len);
    if(fname != NULL) {
        mio_remove(fname);
    }
    return 0;
}
//Lua: mv("old path","new path")
static int lflash_mv( lua_State* L ) {
    const char *oph,*nph;
    size_t len;
    oph = luaL_checklstring(L, 1, &len);
    nph = luaL_checklstring(L, 2, &len);
    if(oph != NULL && nph != NULL) {
        mio_rename(oph,nph);
    }
    return 0;
}

//Lua: cat("fname")
static int lflash_cat( lua_State* L ) {
    const char *fname;
    char *buffer;
    size_t len;
    FILE *fp;
    buffer = malloc(100);
    fname = luaL_checklstring(L, 1, &len);
    fp = mio_fopen(fname,"r");
    while(!mio_feof(fp)){
        mio_fgets(buffer,100,fp);
        lua_writestring(buffer,1);
        lua_writestring("\r\n",1);
    }
    free(buffer);
    return 0;
}

//Lua: transfer("fname",PC2MCU)
static int lflash_transfer( lua_State* L ) {
    const char *fname;
    size_t len;
    int type;
    fname = luaL_checklstring( L, 1, &len );
    type  = luaL_checkinteger( L, 2 );
    Lua_InPutOutPutDisable();
    if(type == PC2MCU) {
        int res = ent_recfile(fname);
        if( res < 0 ) {
            luaL_error( L, "Creat File Error" );
        }
    }
    else {
        int res = ent_senfile(fname);
        if( res < 0 ) {
            luaL_error( L, "Open File Error" );
        }
    }
    return 0;
}

// Module function map
static const luaL_Reg flash_map[] = {
  {"format"   ,lflash_format  },
  {"info"     ,lflash_info    },
  {"ls"       ,lflash_ls      },
  {"rm"       ,lflash_rm      },
  {"mv"       ,lflash_mv      },
  {"cat"      ,lflash_cat     },
  {"transfer" ,lflash_transfer},
  
  
  {"PC2MCU",NULL},
  {"MCU2PC" ,NULL},
  
  { NULL, NULL }
};


LUAMOD_API int luaopen_flash (lua_State *L) {
    luaL_newlib(L, flash_map);
    lua_pushinteger(L, PC2MCU);
    lua_setfield(L, -2, "PC2MCU");
    lua_pushinteger(L, MCU2PC);
    lua_setfield(L, -2, "MCU2PC");
    return 1;
}


