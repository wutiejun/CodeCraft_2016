#ifndef _ZEBRA_MEMORY_H
#define _ZEBRA_MEMORY_H


/* For pretty printing of memory allocate information. */
struct memory_list
{
    const char *format;
};

#include "lib/memtypes.h"


#define XMALLOC(mtype, size)       zebra_zmalloc (__FILE__, __LINE__, (mtype), (size))
#define XCALLOC(mtype, size)       zebra_zcalloc (__FILE__, __LINE__, (mtype), (size))
#define XREALLOC(mtype, ptr, oldsize, size) \
    zebra_zrealloc (__FILE__, __LINE__, (mtype), (ptr), (oldsize), (size))
#define XFREE(mtype, ptr)          do { \
                                     zebra_zfree (__FILE__, __LINE__, (mtype), (ptr)); \
                                     ptr = NULL; } \
                                   while (0)
#define XSTRDUP(mtype, str)        zebra_zstrdup (__FILE__, __LINE__, (mtype), (str))

/* Prototypes of memory function. */
extern void *zebra_zmalloc (char *funname, int line, int type, size_t size);
extern void *zebra_zcalloc (char *funname, int line, int type, size_t size);
extern void *zebra_zrealloc (char *funname, int line, int type, void *ptr, size_t old, size_t size);
extern void  zebra_zfree (char *funname, int line, int type, void *ptr);
extern char *zebra_zstrdup (char *funname, int line, int type, const char *str);

extern void *zebra_mtype_zmalloc (const char *file, int line, int type, size_t size);

extern void *zebra_mtype_zcalloc (const char *file, int line, int type, size_t size);

extern void *zebra_mtype_zrealloc (const char *file, int line, int type, void *ptr,
                                   size_t old, size_t size);

extern void zebra_mtype_zfree (const char *file, int line, int type,
                               void *ptr);

extern char *zebra_mtype_zstrdup (const char *file, int line, int type,
                                  const char *str);
#include "vector.h"
extern void zebra_memory_init (vector cmdvec);
extern void zebra_log_memstats_stderr (const char *);

/* return number of allocations outstanding for the type */
extern unsigned long zebra_mtype_stats_alloc (int);

/* Human friendly string for given byte count */
#define MTYPE_MEMSTR_LEN 20
extern const char *zebra_mtype_memstr (char *, size_t, unsigned long);

char * zebra_strdup(const char *s);

#endif /* _ZEBRA_MEMORY_H */
