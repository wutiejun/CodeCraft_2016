
#include <base.h>
/* malloc.h is generally obsolete, however GNU Libc mallinfo wants it. */
#if !defined(HAVE_STDLIB_H) || (defined(GNU_LINUX) && defined(HAVE_MALLINFO))
#include <malloc.h>
#endif /* !HAVE_STDLIB_H || HAVE_MALLINFO */

#include "memory.h"

#define zMalloc(size) malloc(size)
#define zRealloc(p,size) realloc(p,size)
#define zFree(p) free(p)

static void zebra_alloc_inc (int);
static void zebra_alloc_dec (int);

static void
zebra_zerror (const char *fname, int type, size_t size)
{
    printf("%s : can't allocate memory for `%d' size %d: %d\n",
                    fname, type, (int) size, errno);
}

/*
 * Allocate memory of a given size, to be tracked by a given type.
 * Effects: Returns a pointer to usable memory.  If memory cannot
 * be allocated, aborts execution.
 */

void *
zebra_zmalloc (char *funname, int line, int type, size_t size)
{
    void *memory;

    memory = zMalloc(size);

    if (memory == NULL)
        zebra_zerror ("malloc", type, size);
    else
        zebra_alloc_inc (type);

    return memory;
}

/*
 * Allocate memory as in zebra_zmalloc, and also clear the memory.
 */
void *
zebra_zcalloc (char *funname, int line, int type, size_t size)
{
    void *memory;

    memory = zMalloc(size);
    if (memory == NULL)
    {
        zebra_zerror ("calloc", type, size);
    }
    else
    {
        memset(memory, 0, size);
        zebra_alloc_inc (type);
    }

    return memory;
}

/*
 * Given a pointer returned by zebra_zmalloc or zebra_zcalloc, free it and
 * return a pointer to a new size, basically acting like realloc().
 * Requires: ptr was returned by zebra_zmalloc, zebra_zcalloc, or zebra_zrealloc with the
 * same type.
 * Effects: Returns a pointer to the new memory, or aborts.
 */
void *
zebra_zrealloc (char *funname, int line, int type, void *ptr, size_t oldsize, size_t size)
{
    void *memory;

    memory = zMalloc(size);
    if(memory != NULL)
    {
        memset(memory, 0, size);
        zebra_alloc_inc (type);
        if(ptr != NULL)
        {
            memcpy(memory, ptr, oldsize);
            zFree(ptr);
            zebra_alloc_dec (type);
        }
    }

    if (memory == NULL)
        zebra_zerror ("realloc", type, size);

    return memory;
}

/*
 * Free memory allocated by z*alloc or zebra_zstrdup.
 * Requires: ptr was returned by zebra_zmalloc, zebra_zcalloc, or zebra_zrealloc with the
 * same type.
 * Effects: The memory is freed and may no longer be referenced.
 */
void
zebra_zfree (char *funname, int line, int type, void *ptr)
{
    if (ptr != NULL)
    {
        zebra_alloc_dec (type);
        zFree(ptr);
    }
}

/*
 * Duplicate a string, counting memory usage by type.
 * Effects: The string is duplicated, and the return value must
 * eventually be passed to zebra_zfree with the same type.  The function will
 * succeed or abort.
 */
char *
zebra_zstrdup (char *funname, int line, int type, const char *str)
{
    char *new;

    if ((str == NULL))
            return NULL;
    new = zMalloc (strlen(str) + 1);
    if (new == NULL)
        return NULL;

    strcpy (new, str);

    zebra_alloc_inc (type);
    return new;

}

static struct
{
    char *name;
    long alloc;
} mstat [MTYPE_MAX];

/* Increment allocation counter. */
static void
zebra_alloc_inc (int type)
{
    mstat[type].alloc++;
}

/* Decrement allocation counter. */
static void
zebra_alloc_dec (int type)
{
    mstat[type].alloc--;
}

/* Looking up memory status from vty interface. */
#include "vector.h"
struct memory_list memory_name[] =
{
    "MTYPE_VECTOR",                  
    "MTYPE_VECTOR_INDEX",         
    "MTYPE_LINK_LIST",              
    "MTYPE_LINK_NODE",               
    "MTYPE_HASH",                     
    "MTYPE_HASH_BACKET",           
    "MTYPE_HASH_INDEX",
    "MTYPE_PQUEUE",
    "MTYPE_PQUEUE_DATA",
    "MTYPE_VERTEX",           
    "MTYPE_SPFTREE",             
    "MTYPE_TOPO",               
    "MTYPE_PASS_TREE",        
    "MTYPE_PASS_TREENODE",
    "MTYPE_ROUTER",               
    "MTYPE_LINK_RECORD", 
    "MTYPE_PATH",              
    "MTYPE_CPATH",
    "MTYPE_FIFO",
    "MEM_QCCS_SRLG_EXCLUDE",
    "MTYPE_INT",
    "MTYPE_PASSOBJ",
    "MTYPE_NODEID"
    "MTYPE_EDGE",
    "Invalid",                   
    
};


int show_alloc_memory()
{
    int index = 0;
    struct mlist *ml;
    int needsep = 0;
    printf("\r\n-----------memory not free begin------------------\r\n");
    for (index = 0; index < MTYPE_MAX; index++)
    {
      if (mstat[index].alloc)
      {
          printf("%-20s: %10ld\r\n", memory_name[index], mstat[index].alloc);
      }    
    }
    printf("-----------memory not free end-----------------\r\n");

}
