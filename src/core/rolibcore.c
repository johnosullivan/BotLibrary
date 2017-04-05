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

#define lrolibcore_c
#define VERSIONROLIB (l_mathop(1.0))

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
#include <time.h>
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
char const* get_osx_version(void)
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
/* Will list what feature are supported on machine */
static int sys_info (lua_State *L) {
    //Creating grid
    char grid[2048];
    snprintf(grid, sizeof(grid),"%s---------------------------\n|rolibcore          | \x1b[32myes\x1b[0m |\n---------------------------\n|rolibgpio          | \x1b[32myes\x1b[0m |\n---------------------------\n|rolibsensor        | \x1b[32myes\x1b[0m |\n---------------------------\n|rolibservo         | \x1b[32myes\x1b[0m |\n---------------------------","");

    char buffer[2048];
    snprintf(buffer, sizeof(buffer), "===================================================\n\x1b[33m>>> Library Information <<<\x1b[0m\n===================================================\nCore Version: %s\n===================================================\n\x1b[33m>>> System Information <<<\x1b[0m\n===================================================\nUser: %s \nHost: %s \nUptime: %s\nOSX: %s\nKernel %s\nShell %s\nCPU: %s \nRAM: %s \nDisk Space: %s \nGPU: %s\n===================================================\n\x1b[33m>>> Support Chart <<<\x1b[0m \n%s\n===================================================","1.0",get_user(),get_hostname(),get_uptime(),get_osx_version(),get_kernel_version(),get_shell(),get_cpu(),get_ram(), get_disk(),get_gpu(),grid);
    lua_pushstring(L, buffer);
    return 1;
}
/* Library functions */
static const struct luaL_Reg lservo_functions[] = {
    { "sys_info", sys_info},
    { NULL, NULL }
};
/* Init the Lua Robot Library */
LUAMOD_API int luaopen_rolibcore (lua_State *L) {
    luaL_newlib(L, lservo_functions);
    lua_pushnumber(L, VERSIONROLIB);
    lua_setfield(L, -2, "version");
    return 1;
}
