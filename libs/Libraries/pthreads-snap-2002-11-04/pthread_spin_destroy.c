/*
 * pthread_spin_destroy.c
 *
 * Description:
 * This translation unit implements spin lock primitives.
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999,2002 Pthreads-win32 contributors
 * 
 *      Contact Email: rpj@ise.canberra.edu.au
 * 
 *      The current list of contributors is contained
 *      in the file CONTRIBUTORS included with the source
 *      code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      http://sources.redhat.com/pthreads-win32/contributors.html
 * 
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 * 
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 * 
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library in the file COPYING.LIB;
 *      if not, write to the Free Software Foundation, Inc.,
 *      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "pthread.h"
#include "implement.h"


int
pthread_spin_destroy(pthread_spinlock_t *lock)
{
  register pthread_spinlock_t s;

  if (lock == NULL || *lock == NULL)
    {
      return EINVAL;
    }

  if ((s = *lock) != PTHREAD_SPINLOCK_INITIALIZER)
    {
      if (s->interlock == PTW32_SPIN_USE_MUTEX)
        {
          return pthread_mutex_destroy(&(s->u.mutex));
        }

      if ( (PTW32_INTERLOCKED_LONG) PTW32_SPIN_UNLOCKED ==
           ptw32_interlocked_compare_exchange((PTW32_INTERLOCKED_LPLONG) &(s->interlock),
                                              (PTW32_INTERLOCKED_LONG) PTW32_OBJECT_INVALID,
                                              (PTW32_INTERLOCKED_LONG) PTW32_SPIN_UNLOCKED))
        {
          return 0;
        }

      return EINVAL;
    }
  else
    {
      int result = 0;

      /*
       * See notes in ptw32_spinlock_check_need_init() above also.
       */
      EnterCriticalSection(&ptw32_spinlock_test_init_lock);

      /*
       * Check again.
       */
      if (*lock == PTHREAD_SPINLOCK_INITIALIZER)
        {
          /*
           * This is all we need to do to destroy a statically
           * initialised spinlock that has not yet been used (initialised).
           * If we get to here, another thread
           * waiting to initialise this mutex will get an EINVAL.
           */
          *lock = NULL;
        }
      else
        {
          /*
           * The spinlock has been initialised while we were waiting
           * so assume it's in use.
           */
          result = EBUSY;
        }

      LeaveCriticalSection(&ptw32_spinlock_test_init_lock);
      return(result);
    }
}
