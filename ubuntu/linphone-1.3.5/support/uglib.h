/*
linphone
Copyright (C) 2000  Simon MORLAT (simon.morlat@free.fr)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#ifndef UGLIB_H
#define UGLIB_H


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _GNU_SOURCE

#ifdef HAVE_GLIB
#error "uglib.h must not be included when HAVE_GLIB is defined."
#endif

#include "ortp/port.h"

/* integer types */
typedef  uint64_t guint64;
typedef  int64_t gint64;
typedef  uint16_t guint16;
typedef  uint32_t guint32;
typedef  signed short gint16;
typedef  int32_t gint32;
typedef  unsigned int guint;
typedef  int gint;
typedef  char gchar;
typedef  unsigned char guchar;
typedef  unsigned char guint8;
typedef  void* gpointer;
typedef int gboolean;
typedef double gdouble;
typedef int gsize;
typedef ssize_t gssize;
typedef  unsigned long gulong;

#undef TRUE
#undef FALSE
#define TRUE 1
#define FALSE 0

#ifdef __cplusplus
#define G_BEGIN_DECLS extern"C"{
#define G_END_DECLS }
#else
#define G_BEGIN_DECLS 
#define G_END_DECLS 
#endif



typedef gchar GError;
GError *g_error_new(gint dummy1, gint dummy2, const gchar* errormsg,...);
void    g_set_error(GError **err,gint dummy1,gint dummy2,const gchar *format,...);

/*misc*/
#define g_return_if_fail(expr) if (!(expr)) {printf("%s:%i- assertion" #expr "failed\n",__FILE__,__LINE__); return;}
#define g_return_val_if_fail(expr,ret) if (!(expr)) {printf("%s:%i- assertion #expr failed\n",__FILE__,__LINE__); return (ret);}
#define g_assert(expr) if (!(expr)) printf("%s:%i- assertion #expr failed\n",__FILE__,__LINE__)
#define g_assert_not_reached() printf("%s:%i- assert not reached failed\n",__FILE__,__LINE__)


static inline void g_warning(const gchar *fmt,...)
{
  va_list args;
  fprintf(stderr,"Warning:");
  va_start (args, fmt);
  vfprintf (stderr, fmt, args);
  va_end (args);
  fprintf(stderr,"\n");
}
static inline void g_error(const gchar *fmt,...)
{
  va_list args;
  fprintf(stderr,"Error:");
  va_start (args, fmt);
  vfprintf (stderr, fmt, args);
  va_end (args);
  fprintf(stderr,"\n");
  abort();
}

static inline void g_message(const gchar *fmt,...)
{
  va_list args;
  fprintf(stderr,"Message:");
  va_start (args, fmt);
  vfprintf (stderr, fmt, args);
  va_end (args);
  fprintf(stderr,"\n");
}

/* memory allocation functions */
#define g_malloc(sz)  malloc(sz)

static inline void * g_malloc0(int sz)
{
	void *p=malloc(sz);
	memset(p,0,sz);
	return p;
}

#define g_alloca(s)    alloca(s)
#define g_new(type,count)   (type *)g_malloc(sizeof(type)*(count))
#define g_new0(type, count)	(type *)g_malloc0(sizeof(type)*(count))
#define g_realloc(p,sz) realloc((p),(sz))
#define g_free(p) free(p)


typedef ortp_mutex_t GMutex;
static inline GMutex * g_mutex_new()
{
	ortp_mutex_t *mutex=g_new(ortp_mutex_t,1);
	ortp_mutex_init(mutex,NULL);
	return mutex;
}

typedef enum
{
  G_THREAD_PRIORITY_LOW,
  G_THREAD_PRIORITY_NORMAL,
  G_THREAD_PRIORITY_HIGH,
  G_THREAD_PRIORITY_URGENT
} GThreadPriority;

typedef ortp_thread_t GThread;

typedef gpointer (*GThreadFunc)(gpointer data);

static inline GThread *g_thread_create(GThreadFunc func, gpointer data, gboolean joinable, void **error){
	GThread *thread=g_new(GThread,1);
	ortp_thread_create(thread,NULL,func,data);
	return thread;
}

static inline void g_thread_join(GThread *thread){
	ortp_thread_join(*thread,NULL);
	g_free(thread);
}


#define g_mutex_lock(mutex)	  ortp_mutex_lock((mutex))
#define g_mutex_unlock(mutex)  ortp_mutex_unlock((mutex))
static inline void g_mutex_free(GMutex *mutex){
	   ortp_mutex_destroy(mutex);
	   g_free(mutex);
}

typedef ortp_cond_t GCond;
static inline GCond * g_cond_new()
{
	ortp_cond_t *cond=g_new(ortp_cond_t,1);
	ortp_cond_init(cond,NULL);
	return cond;
}
#define g_cond_wait(cond,mutex)	ortp_cond_wait((cond),(mutex))
#define g_cond_signal(cond)		ortp_cond_signal((cond))
#define g_cond_broadcast(cond)	ortp_cond_broadcast((cond))
#define g_cond_free(cond)		ortp_cond_destroy((cond)); g_free((cond))

#define g_thread_init(vtable)
#define g_thread_supported()	(1)

#include <time.h>
typedef struct timeval GTimer;
#define g_timer_new() g_new(GTimer,1)
#define g_timer_destroy(t) g_free((t))
#define g_timer_start(t)  gettimeofday((t),NULL)
#define g_timer_reset(t)  g_timer_start(t)

gdouble g_timer_elapsed(GTimer *t,guint *us);

/*GString */

struct _GString{
	gchar *str;
	gint curindex;
	gint len;
	gint allocated_len;
};
typedef struct _GString GString;

G_BEGIN_DECLS
GString *g_string_new(const gchar *init);
GString *g_string_append_c(GString *string, gchar c);
GString *g_string_append(GString *obj, const gchar *val);
gchar * g_string_free(GString *string, gboolean free_segment);
G_END_DECLS

#include "glist.h"

#define G_GNUC_CONST

G_BEGIN_DECLS
gchar *g_strdup_printf(const char *fmt,...);
gchar* g_strconcat(const gchar *string1, ...);
void g_strfreev(gchar **strarray);

G_END_DECLS
#define g_stpcpy(d,s) stpcpy((d),(s))
#define g_strdup(machin)	strdup(machin)
#define g_strndup(s,n)  strndup((s),(n))
#define g_snprintf  snprintf
#define g_getenv	getenv
#define g_strerror(errnum) strerror(errnum)


//typedef enum {G_FILE_TEST_EXISTS} GFileTest;
typedef enum
{
  G_FILE_TEST_IS_REGULAR    = 1 << 0,
  G_FILE_TEST_IS_SYMLINK    = 1 << 1,
  G_FILE_TEST_IS_DIR        = 1 << 2,
  G_FILE_TEST_IS_EXECUTABLE = 1 << 3,
  G_FILE_TEST_EXISTS        = 1 << 4
} GFileTest;


#define g_get_home_dir() getenv("HOME")
#define g_get_current_dir() get_current_dir_name()

#define g_build_filename(a,b,c) g_strdup_printf("%s/%s",a,b)

typedef enum
{
  /* log flags */
  G_LOG_FLAG_RECURSION          = 1 << 0,
  G_LOG_FLAG_FATAL              = 1 << 1,

  /* GLib log levels */
  G_LOG_LEVEL_ERROR             = 1 << 2,       /* always fatal */
  G_LOG_LEVEL_CRITICAL          = 1 << 3,
  G_LOG_LEVEL_WARNING           = 1 << 4,
  G_LOG_LEVEL_MESSAGE           = 1 << 5,
  G_LOG_LEVEL_INFO              = 1 << 6,
  G_LOG_LEVEL_DEBUG             = 1 << 7,

  G_LOG_LEVEL_MASK              = ~(G_LOG_FLAG_RECURSION | G_LOG_FLAG_FATAL)
} GLogLevelFlags;

#ifndef G_LOG_DOMAIN
#define G_LOG_DOMAIN ""
#endif
void g_log(const gchar *log_domain,GLogLevelFlags log_level,const gchar *format,...);
void g_logv(const gchar *log_domain,GLogLevelFlags log_level,const gchar *format,va_list args);
typedef void (*GLogFunc)  (const gchar *log_domain,
                                          GLogLevelFlags log_level,
                                           const gchar *message,
                                           gpointer user_data);
void g_log_set_handler(const gchar *log_domain,GLogLevelFlags log_levels, GLogFunc log_func, gpointer user_data);


#include "gspawn.h"
#include "gshell.h"

#define GINT_TO_POINTER(a)	((void*)(long)a)

gboolean g_file_test(const char *filename, gint test);

#ifndef HAVE_STRNDUP
char *strndup(const char *s, size_t n);
#endif

#ifndef HAVE_GET_CURRENT_DIR_NAME
char *get_current_dir_name(void);
#endif

#ifndef HAVE_STPCPY
char *stpcpy(char *dest, const char *src);
#endif

#endif

guint32 g_random_int(void);
