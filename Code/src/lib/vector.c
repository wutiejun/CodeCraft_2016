/* Generic vector interface routine
 * Copyright (C) 1997 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <base.h>

#include "vector.h"
#include "memory.h"

/* Initialize vector : allocate memory and return vector. */
vector
zebra_vector_init (unsigned int size)
{
    vector v = XCALLOC (MTYPE_VECTOR, sizeof (struct _vector));

    if(NULL == v)
    {
        printf("\r\nzebra_vector_init-malloc failed.");
        return NULL;
    }
    memset(v, 0, sizeof (struct _vector));

    v->index = XCALLOC (MTYPE_VECTOR_INDEX, sizeof (void *) * size);
    if(v->index == NULL)
    {
        XFREE (MTYPE_VECTOR, v);
        return NULL;
    }
    /* allocate at least one slot */
    if (size == 0)
        size = 1;

    v->alloced = size;
    v->active = 0;
    return v;
}

void
zebra_vector_only_wrapper_free (vector v)
{
    if(v != NULL)XFREE (MTYPE_VECTOR, v);
}

void
zebra_vector_only_index_free (void *index)
{
    if(index != NULL)XFREE (MTYPE_VECTOR_INDEX, index);
}

void
zebra_vector_free (vector v)
{
    if(v->index != NULL)XFREE (MTYPE_VECTOR_INDEX, v->index);
    if(v != NULL)XFREE (MTYPE_VECTOR, v);
}

vector
zebra_vector_copy (vector v)
{
    unsigned int size;
    vector new;

    if(v == NULL)return NULL;

    new = XCALLOC (MTYPE_VECTOR, sizeof (struct _vector));
    if(NULL == new)
    {
        printf("\r\nzebra_vector_copy-malloc failed.");
        return NULL;
    }
    memset(new, 0, sizeof (struct _vector));

    size = sizeof (void *) * (v->alloced);
    new->index = XCALLOC (MTYPE_VECTOR_INDEX, size);
    if(new->index == NULL)
    {
        XFREE(MTYPE_VECTOR,new);
        return NULL;
    }
    new->active = v->active;
    new->alloced = v->alloced;
    memcpy (new->index, v->index, size);

    return new;
}

/* Check assigned index, and if it runs short double index pointer */
void
zebra_vector_ensure (vector v, unsigned int num)
{
    if (v == NULL || v->alloced > num)
        return;

    v->index = XREALLOC (MTYPE_VECTOR_INDEX,
                         v->index,
                         sizeof (void *) * (v->alloced),
                         sizeof (void *) * (v->alloced * 2));
    if(v->index == NULL)return;

    memset (&v->index[v->alloced], 0, sizeof (void *) * v->alloced);
    v->alloced *= 2;

    if (v->alloced <= num)
        zebra_vector_ensure (v, num);
}

/* This function only returns next empty slot index.  It dose not mean
   the slot's index memory is assigned, please call zebra_vector_ensure()
   after calling this function. */
int
zebra_vector_empty_slot (vector v)
{
    unsigned int i;

    if (v == NULL || v->active == 0)
        return 0;

    for (i = 0; i < v->active; i++)
        if (v->index[i] == 0)
            return i;

    return i;
}

/* Set value to the smallest empty slot. */
int
zebra_vector_set (vector v, void *val)
{
    unsigned int i;

    i = zebra_vector_empty_slot (v);
    zebra_vector_ensure (v, i);

    v->index[i] = val;

    if (v->active <= i)
        v->active = i + 1;

    return i;
}

/* Set value to specified index slot. */
int
zebra_vector_set_index (vector v, unsigned int i, void *val)
{
    zebra_vector_ensure (v, i);

    v->index[i] = val;

    if (v->active <= i)
        v->active = i + 1;

    return i;
}

/* Look up vector.  */
void *
zebra_vector_lookup (vector v, unsigned int i)
{
    if (i >= v->active)
        return NULL;
    return v->index[i];
}

/* Lookup vector, ensure it. */
void *
zebra_vector_lookup_ensure (vector v, unsigned int i)
{
    zebra_vector_ensure (v, i);
    return v->index[i];
}

/* Unset value at specified index slot. */
void
zebra_vector_unset (vector v, unsigned int i)
{
    if (i >= v->alloced)
        return;

    v->index[i] = NULL;

    if (i + 1 == v->active)
    {
        v->active--;
        while (i && v->index[--i] == NULL && v->active--)
            ;				/* Is this ugly ? */
    }
}

/* Count the number of not emplty slot. */
unsigned int
zebra_vector_count (vector v)
{
    unsigned int i;
    unsigned count = 0;
    if(v == NULL)return count;

    for (i = 0; i < v->active; i++)
        if (v->index[i] != NULL)
            count++;

    return count;
}
