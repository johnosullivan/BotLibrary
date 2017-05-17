/*
 Copyright 2017 John O'Sullivan

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#define lbotlibcore_c
#define VERSIONbotlib (l_mathop(1.0))

/* Linking Frameworks */
#include <lua.h>
#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#ifdef __APPLE__
#include <time.h>
#elif __linux__
#include <sys/time.h>
#endif
#include <sys/utsname.h>
#include <sys/sysctl.h>
#include <sys/param.h>
#include <sys/mount.h>

#ifdef _WIN32
#define O_NOCTTY 0
#else
#include <termios.h>
#endif

#define CLEAR     "\x1b[0m"
#define RED       "\x1b[31m"
#define GREEN     "\x1b[32m"

#include "../gpio/gpio.h"

/* Defining Vars */
#undef PI
#define PI  (l_mathop(3.141592653589793238462643383279502884))
#if !defined(l_rand)
#if defined(LUA_USE_POSIX)
#define l_rand()    random()
#define l_srand(x)  srandom(x)
#define L_RANDMAX   2147483647
#else
#define l_rand()    rand()
#define l_srand(x)  srand(x)
#define L_RANDMAX   RAND_MAX
#endif
#endif

/* get username */
char const* get_user(void)
{
  static char ret[255];
  #ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
  #elif __APPLE__
    return getlogin();
  #elif __linux__
    // linux
  #elif __unix__ // all unices not caught above
    // Unix
  #elif defined(_POSIX_VERSION)
    // POSIX
  #else
  #   error "Unknown compiler"
  #endif
  return ret;
}
/* get loalhost name */
char const* get_hostname(void)
{
  static char ret[255];
  #ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
  #elif __APPLE__
  if (gethostname(ret, 255))
  {
    return NULL;
  }
  #elif __linux__
    // linux
  #elif __unix__ // all unices not caught above
    // Unix
  #elif defined(_POSIX_VERSION)
    // POSIX
  #else
  #   error "Unknown compiler"
  #endif
  return ret;
}
/* gets the operating system */
char const* get_operating_version(void)
{
  static char ret[255];
  #ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
  #elif __APPLE__
  FILE* fp;
	int versionlen;
	char version[64];
	size_t osversionlen;
	char osversion[64];
	fp = popen("/usr/bin/sw_vers -productVersion", "r");
	if (!fp) return NULL;
	if (!fgets(version, sizeof(version) - 1, fp))
  {
    pclose(fp);
    return NULL;
  }
	pclose(fp);
  osversionlen = sizeof(osversion);
  if (sysctlbyname("kern.osversion", osversion, &osversionlen, NULL, 0)) {
    return NULL;
  }
  versionlen = strlen(version);
  snprintf(ret, 255, "%.*s (%s)", versionlen - 1, version, osversion);
  #elif __linux__

  #elif __unix__
    // Unix
  #elif defined(_POSIX_VERSION)
    // POSIX
  #else
  #   error "Unknown compiler"
  #endif
  return ret;
}
/* gets the kernal name */
char const* get_kernel_version(void)
{
  static char ret[255];
  #ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
  #elif __APPLE__
  struct utsname sys;
  uname(&sys);
  if ((strlen(sys.sysname) + strlen(sys.version)) > 255) {
    return NULL;
  }
  snprintf(ret, 255, "%s %s", sys.sysname, sys.release);
  #elif __linux__
    // linux
  #elif __unix__ // all unices not caught above
    // Unix
  #elif defined(_POSIX_VERSION)
    // POSIX
  #else
  #   error "Unknown compiler"
  #endif
  return ret;
}
/* Gets the uptime */
char const* get_uptime(void)
{
  static char ret[255];
  #ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
  #elif __APPLE__
  struct timeval boottime;
  size_t len = sizeof(boottime);
  time_t diff;
  time_t days, hours, minutes;
  if (sysctlbyname("kern.boottime", &boottime, &len, NULL, 0)) {
        return NULL;
  }
  diff = (time_t)difftime(time(NULL), boottime.tv_sec);
  days = diff / (60*60*24);
	hours = (diff / (60*60)) % 24;
	minutes = (diff / 60) % 60;
	snprintf(ret, 255, "%ld days, %ld hours and %ld minutes", days, hours, minutes);
  #elif __linux__
    // linux
  #elif __unix__ // all unices not caught above
    // Unix
  #elif defined(_POSIX_VERSION)
    // POSIX
  #else
  #   error "Unknown compiler"
  #endif
  return ret;
}
/* Gets the shell name */
char const* get_shell(void)
{
  static char ret[255];
  #ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
  #elif __APPLE__
  char* env;
  env = getenv("SHELL");
  memcpy(ret, env, strlen(env));
  #elif __linux__
    // linux
  #elif __unix__ // all unices not caught above
    // Unix
  #elif defined(_POSIX_VERSION)
    // POSIX
  #else
  #   error "Unknown compiler"
  #endif
  return ret;
}
/* Get terminl name */
char const* get_terminal(void)
{
  static char ret[255];
  #ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
  #elif __APPLE__
  char* env;
  env = getenv("TERM");
  memcpy(ret, env, strlen(env));
  #elif __linux__
    // linux
  #elif __unix__ // all unices not caught above
    // Unix
  #elif defined(_POSIX_VERSION)
    // POSIX
  #else
  #   error "Unknown compiler"
  #endif
  return ret;
}
/* Gets the CPU type */
char const* get_cpu(void)
{
  static char ret[255];
  #ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
  #elif __APPLE__
  size_t len = 255;
  if (sysctlbyname("machdep.cpu.brand_string", ret, &len, NULL, 0)) {
    return NULL;
  }
  #elif __linux__
    // linux
  #elif __unix__ // all unices not caught above
    // Unix
  #elif defined(_POSIX_VERSION)
    // POSIX
  #else
  #   error "Unknown compiler"
  #endif
  return ret;
}
/* Gets the GPU type */
char const* get_gpu(void)
{
  static char ret[255];
  #ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
  #elif __APPLE__
  FILE* fp;
  char output[255];
  size_t i, len;
  char* cmd = "system_profiler -detailLevel mini SPDisplaysDataType 2> /dev/null | grep \"Chipset Model:\" | sed -e 's/Chipset Model://' | sed -e 's/^[ \\t ]*//'";
  fp = popen(cmd, "r");
	if (!fp) return NULL;
	if ((len = fread(output, 1, sizeof(output) - 1, fp)) == 0)
  {
      pclose(fp);
      return NULL;
  }
	pclose(fp);
  output[len] = '\0';
  len = strlen(output);
  memcpy(ret, output, len);
  for (i = 0; i < len; i++)
  {
      if (ret[i] == '\n')
      {
          ret[i] = ',';
      }
  }
  ret[len - 1] = '\0';
  #elif __linux__
    // linux
  #elif __unix__ // all unices not caught above
    // Unix
  #elif defined(_POSIX_VERSION)
    // POSIX
  #else
  #   error "Unknown compiler"
  #endif
  return ret;
}
/* Gets the RAM */
char const* get_ram(void)
{
  static char ret[255];
  #ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
  #elif __APPLE__
  int64_t mem;
  size_t len = 255;
  if (sysctlbyname("hw.memsize", &mem, &len, NULL, 0)) {
    return NULL;
  }
  mem = mem / 1024 / 1024 / 1024;
  snprintf(ret, 255, "%lld GiB", mem);
  #elif __linux__
    // linux
  #elif __unix__ // all unices not caught above
    // Unix
  #elif defined(_POSIX_VERSION)
    // POSIX
  #else
  #   error "Unknown compiler"
  #endif
  return ret;
}
/* Gets the disk size */
char const* get_disk(void)
{
  static char ret[255];
  #ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
  #elif __APPLE__
  struct statfs fs;
  double free, all;
	statfs("/", &fs);
	free = (fs.f_bsize * fs.f_bfree) / 1000 / 1000 / 1000;
	all = (fs.f_bsize * fs.f_blocks) / 1000 / 1000 / 1000;
	snprintf(ret, 255, "%.f GB / %.f GB", free, all);
  #elif __linux__
    // linux
  #elif __unix__ // all unices not caught above
    // Unix
  #elif defined(_POSIX_VERSION)
    // POSIX
  #else
  #   error "Unknown compiler"
  #endif
  return ret;
}
//Gettime in ms
#ifdef _WIN32
double gettime_ms(void) {
    FILETIME ft;
    double t;
    GetSystemTimeAsFileTime(&ft);
    /* Windows file time (time since January 1, 1601 (UTC)) */
    t  = ft.dwLowDateTime/1.0e7 + ft.dwHighDateTime*(4294967296.0/1.0e7);
    /* convert to Unix Epoch time (time since January 1, 1970 (UTC)) */
    return (t - 11644473600.0);
}
#else
double gettime_ms(void) {
    struct timeval v;
    gettimeofday(&v, (struct timezone *) NULL);
    /* Unix Epoch time (time since January 1, 1970 (UTC)) */
    return v.tv_sec + v.tv_usec/1.0e6;
}
#endif

static int lua_gettime(lua_State *L)
{
    lua_pushnumber(L, gettime_ms());
    return 1;
}
/* Will list what feature are supported on machine */
static int sys_info (lua_State *L) {
    //Creating grid
    char grid[2048];
    snprintf(grid, sizeof(grid),"%s---------------------------\n|botlibcore          | \x1b[32myes\x1b[0m |\n---------------------------\n|botlibgpio          | \x1b[32myes\x1b[0m |\n---------------------------\n|botlibsensor        | \x1b[32myes\x1b[0m |\n---------------------------\n|botlibservo         | \x1b[32myes\x1b[0m |\n---------------------------","");

    char buffer[2048];
    snprintf(buffer, sizeof(buffer), "===================================================\n\x1b[33m>>> Library Information <<<\x1b[0m\n===================================================\nCore Version: %s\n===================================================\n\x1b[33m>>> System Information <<<\x1b[0m\n===================================================\nUser: %s \nHost: %s \nUptime: %s\nOSX: %s\nKernel %s\nShell %s\nCPU: %s \nRAM: %s \nDisk Space: %s \nGPU: %s\n===================================================\n\x1b[33m>>> Support Chart <<<\x1b[0m \n%s\n===================================================","1.0",get_user(),get_hostname(),get_uptime(),get_operating_version(),get_kernel_version(),get_shell(),get_cpu(),get_ram(), get_disk(),get_gpu(),grid);
    lua_pushstring(L, buffer);
    return 1;
}

static int lua_read (lua_State *L) {

    gpio_t gpio_in, gpio_out;
    double start, stop;

    if (pin_open(&gpio_in, 24, GPIO_DIRECTION_IN) < 0) {
      fprintf(stderr, "pin_open(): %s\n", pin_errmsg(&gpio_in));
      exit(1);
    }

    if (pin_open(&gpio_out, 18, GPIO_DIRECTION_OUT) < 0) {
      fprintf(stderr, "pin_open(): %s\n", pin_errmsg(&gpio_out));
      exit(1);
    }

    if (pin_write(&gpio_out, true) < 0) {
        fprintf(stderr, "pin_write(): %s\n", pin_errmsg(&gpio_out));
        exit(1);
    }

    sleep((unsigned int)0.00001);

    if (pin_write(&gpio_out, false) < 0) {
        fprintf(stderr, "pin_write(): %s\n", pin_errmsg(&gpio_out));
        exit(1);
    }


    bool echo = false;
    start = gettime_ms();
    stop = gettime_ms();


    while (echo == false) {
      start = gettime_ms();
      if (pin_read(&gpio_in, &echo) < 0) {
        fprintf(stderr, "pin_read(): %s\n", pin_errmsg(&gpio_in));
        exit(1);
      }
    };

    while (echo == true) {
      stop = gettime_ms();
      if (pin_read(&gpio_in, &echo) < 0) {
        fprintf(stderr, "pin_read(): %s\n", pin_errmsg(&gpio_in));
        exit(1);
      }
    }


    double timeelapsed = stop - start;

    double distance = (timeelapsed * 34300) / 2;


    pin_close(&gpio_in);
    pin_close(&gpio_out);

    lua_pushnumber(L, distance);

    return 1;
}

static int lua_table (lua_State *L) {

  /*lua_createtable(L, 0, 4);

lua_pushstring(L, "name");
lua_pushstring(L, "name1");
lua_settable(L, -3);

lua_pushstring(L, "date");
lua_pushstring(L, "date1");
lua_settable(L, -3);

lua_pushstring(L, "ip");
lua_pushstring(L, "ip1");
lua_settable(L, -3);

lua_pushstring(L, "custom");
lua_pushstring(L, "custon1");
lua_settable(L, -3);*/
   double z;
   lua_getglobal(L, "callback");
   if(!lua_isfunction(L,-1))
   {
       lua_pop(L,1);
       return -1;
   }
   lua_pushnumber(L, 21);   /* push 1st argument */
   lua_pushnumber(L, 31);   /* push 2nd argument */

   /* do the call (2 arguments, 1 result) */
   if (lua_pcall(L, 2, 1, 0) != 0) {
       printf("error running function `f': %s\n",lua_tostring(L, -1));
       return -1;
   }

   /* retrieve result */
   if (!lua_isnumber(L, -1)) {
       printf("function `f' must return a number\n");
       return -1;
   }
   z = lua_tonumber(L, -1);
   printf("Result: %f\n",z);
   lua_pop(L, 1);

    return 1;
}

/* Library functions */
static const struct luaL_Reg lservo_functions[] = {
    { "sys_info", sys_info},
    { "gettime", lua_gettime},
    { "read", lua_read},
    { "test", lua_table},
    { NULL, NULL }
};
/* Init the Lua Robot Library */
LUAMOD_API int luaopen_botlibcore (lua_State *L) {
    luaL_newlib(L, lservo_functions);
    lua_pushnumber(L, VERSIONbotlib);
    lua_setfield(L, -2, "version");
    return 1;
}
