/*!
 * \file        sccp_lock.h
 * \brief       SCCP Lock Header
 * \author      Federico Santulli <fsantulli [at] users.sourceforge.net>
 * \note        Mutex lock code derived from Asterisk 1.4
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *              See the LICENSE file at the top of the source tree.
 *
 * $Date: 2013-04-07 02:01:26 +0200 (Sun, 07 Apr 2013) $
 * $Revision: 4545 $
 */

#ifndef __SCCP_LOCK_H
#  define __SCCP_LOCK_H

#  define sccp_mutex_init(x)          		pbx_mutex_init(x)
#  define sccp_mutex_destroy(x)       		pbx_mutex_destroy(x)

#  if ASTERISK_VERSION_NUMBER >= 10400										/* Channel Mutex Macros for Asterisk 1.4 and above */
#    define sccp_pbx_channel_lock(x)    	pbx_channel_lock(x)
#    define sccp_pbx_channel_unlock(x)  	pbx_channel_unlock(x)
#    define sccp_pbx_channel_trylock(x) 	pbx_channel_trylock(x)
#    if ASTERISK_VERSION_NUMBER >= 10600
#      define AST_CHANNEL_DEADLOCK_AVOIDANCE(x)	CHANNEL_DEADLOCK_AVOIDANCE(x)
#    else
#      define AST_CHANNEL_DEADLOCK_AVOIDANCE(x)	DEADLOCK_AVOIDANCE(&x->lock)
#    endif
#  else	/* ASTERISK_VERSION_NUMBER >= 10400 */									/* Channel Mutex Macros for Asterisk 1.2 */
#    define sccp_pbx_channel_lock(x)    	sccp_mutex_lock(&x->lock)
#    define sccp_pbx_channel_unlock(x) 		sccp_mutex_unlock(&x->lock)
#    define sccp_pbx_channel_trylock(x) 	sccp_mutex_trylock(&x->lock)
#    define AST_CHANNEL_DEADLOCK_AVOIDANCE(x)	do { \
	                                               sccp_pbx_channel_unlock(x); \
		                                       usleep(1); \
		                                       sccp_pbx_channel_lock(x); \
		                                while(0)
#    define AST_DEADLOCK_AVOIDANCE(x)		DEADLOCK_AVOIDANCE(&x->lock)
#  endif													/* ASTERISK_VERSION_NUMBER >= 10400 */

#  ifndef CS_AST_DEBUG_CHANNEL_LOCKS
	/* Macro for Generic Mutex */
#    define sccp_mutex_lock(x)			pbx_mutex_lock(x)
#    define sccp_mutex_lock_desc(x,y) 		pbx_mutex_lock(x)
#    define sccp_mutex_unlock(x)		pbx_mutex_unlock(x)
#    define sccp_mutex_trylock(x)		pbx_mutex_trylock(x)

	/* Macro for Sessions */
#    define sccp_session_lock(x)		pbx_mutex_lock(&x->lock)
#    define sccp_session_unlock(x)		pbx_mutex_unlock(&x->lock)
#    define sccp_session_trylock(x)		pbx_mutex_trylock(&x->lock)

	/* Macro for Globals */
#    define sccp_globals_lock(x)		pbx_mutex_lock(&sccp_globals->x)
#    define sccp_globals_unlock(x)		pbx_mutex_unlock(&sccp_globals->x)
#    define sccp_globals_trylock(x)		pbx_mutex_trylock(&sccp_globals->x)

	/* Macro for Lists */
#    define SCCP_LIST_LOCK(x)			pbx_mutex_lock(&(x)->lock)
#    define SCCP_LIST_UNLOCK(x)			pbx_mutex_unlock(&(x)->lock)
#    define SCCP_LIST_TRYLOCK(x)		pbx_mutex_trylock(&(x)->lock)

	/* Macro for read/write Lists */
#    define SCCP_RWLIST_RDLOCK(x)		pbx_rwlock_rdlock(&(x)->lock)
#    define SCCP_RWLIST_WRLOCK(x)		pbx_rwlock_wrlock(&(x)->lock)
#    define SCCP_RWLIST_UNLOCK(x)		pbx_rwlock_unlock(&(x)->lock)
#    define SCCP_RWLIST_TRYLOCK(x)		pbx_rwlock_trylock(&(x)->lock)
#    define SCCP_RWLIST_TRYRDLOCK(x)		pbx_rwlock_tryrdlock(&(x)->lock)
#    define SCCP_RWLIST_TRYWRLOCK(x)		pbx_rwlock_trywrlock(&(x)->lock)
#  else														/* CS_AST_DEBUG_CHANNEL_LOCKS */

// ugly fix to resolve include order incase of CS_AST_DEBUG_CHANNEL_LOCKS       
#    if ASTERISK_VERSION_NUMBER >= 10400
#      include <asterisk.h>
#    endif
#    include <asterisk/lock.h>
int __sccp_mutex_lock(ast_mutex_t * p_ast_mutex, const char *itemnametolock, const char *filename, int lineno, const char *func);
int __sccp_mutex_unlock(ast_mutex_t * p_ast_mutex, const char *itemnametolock, const char *filename, int lineno, const char *func);
int __sccp_mutex_trylock(ast_mutex_t * p_ast_mutex, const char *itemnametolock, const char *filename, int lineno, const char *func);

#    define sccp_mutex_lock(a)          	__sccp_mutex_lock(a, "(sccp unspecified [" #a "])", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#    define sccp_mutex_lock_desc(a, b) 		__sccp_mutex_lock(a, b, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#    define sccp_mutex_unlock(a)       	 	__sccp_mutex_unlock(a, "(sccp unspecified [" #a "])", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#    define sccp_mutex_trylock(a)       	__sccp_mutex_trylock(a, "(sccp unspecified [" #a "])", __FILE__, __LINE__, __PRETTY_FUNCTION__)

	/* Macro for Sessions */
#    define sccp_session_lock(a)		__sccp_mutex_lock(&a->lock, "(sccp session [" #a "])", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#    define sccp_session_unlock(a)		__sccp_mutex_unlock(&a->lock, "(sccp session [" #a "])", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#    define sccp_session_trylock(a)		__sccp_mutex_trylock(&a->lock, "(sccp session [" #a "])", __FILE__, __LINE__, __PRETTY_FUNCTION__)

	/* Macro for Globals */
#    define sccp_globals_lock(a)		__sccp_mutex_lock(&sccp_globals->a, "(sccp globals [" #a "])", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#    define sccp_globals_unlock(a)		__sccp_mutex_unlock(&sccp_globals->a, "(sccp globals [" #a "])", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#    define sccp_globals_trylock(a)		__sccp_mutex_trylock(&sccp_globals->a, "(sccp globals [" #a "])", __FILE__, __LINE__, __PRETTY_FUNCTION__)

	/* Macro for Lists */
#    define SCCP_LIST_LOCK(a)			__sccp_mutex_lock(&(a)->lock, "(SCCP LIST [" #a "])", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#    define SCCP_LIST_UNLOCK(a)			__sccp_mutex_unlock(&(a)->lock, "(SCCP LIST [" #a "])", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#    define SCCP_LIST_TRYLOCK(a)		__sccp_mutex_trylock(&(a)->lock, "(SCCP LIST [" #a "])", __FILE__, __LINE__, __PRETTY_FUNCTION__)

	/* Macro for read/write Lists */
	/* \todo add __sccp_rwlock implementation for debugging in sccp_lock.c */
#    define SCCP_RWLIST_RDLOCK(x)		pbx_rwlock_rdlock(&(x)->lock)
#    define SCCP_RWLIST_WRLOCK(x)		pbx_rwlock_wrlock(&(x)->lock)
#    define SCCP_RWLIST_UNLOCK(x)		pbx_rwlock_unlock(&(x)->lock)
#    define SCCP_RWLIST_TRYLOCK(x)		pbx_rwlock_trylock(&(x)->lock)
#    define SCCP_RWLIST_TRYRDLOCK(x)		pbx_rwlock_tryrdlock(&(x)->lock)
#    define SCCP_RWLIST_TRYWRLOCK(x)		pbx_rwlock_trywrlock(&(x)->lock)
#  endif													/* CS_AST_DEBUG_CHANNEL_LOCKS */

// Declare CAS32 / CAS_PTR and CAS_TYPE for easy reference in other functions
#  ifdef SCCP_ATOMIC

#    ifdef SCCP_BUILTIN_INCR
#      define ATOMIC_INCR(_a,_b,_c) 		__sync_fetch_and_add(_a,_b)
#      define ATOMIC_DECR(_a,_b,_c) 		__sync_fetch_and_add(_a,-_b)
#    else
#      define ATOMIC_INCR(_a,_b,_c) 		AO_fetch_and_add(_a,_b)
#      define ATOMIC_DECR(_a,_b,_c) 		AO_fetch_and_add(_a,-_b)
#    endif

#    ifdef SCCP_BUILTIN_CAS32
#      define CAS32(_a,_b,_c, _d) 		__sync_val_compare_and_swap(_a, _b, _c)
#      define CAS32_TYPE int
#    else
#      define CAS32(_a,_b,_c, _d) 		AO_compare_and_swap(_a, _b, _c)
#      define CAS32_TYPE int
#    endif

#    ifdef SCCP_BUILTIN_CAS_PTR
#      define CAS_PTR(_a,_b,_c, _d) 		__sync_bool_compare_and_swap(_a, _b, _c)
#    else
#      define CAS_PTR(_a,_b,_c, _d) 		AO_compare_and_swap((uintptr_t *)_a, (uintptr_t)_b, _(uintptr_t)_c)
#    endif

#  else														/* SCCP_ATOMIC */

#    define CAS32_TYPE 				int
#    define ATOMIC_INCR(_a,_b,_c)			\
	({					\
                CAS32_TYPE res=0;		\
                ast_mutex_lock(_c);		\
                res = *_a;			\
                *_a += _b;			\
                ast_mutex_unlock(_c);		\
                res;				\
	})
#    define ATOMIC_DECR(_a,_b,_c) 		ATOMIC_INCR(_a,-_b,_c)
#    define CAS32(_a,_b,_c,_d)			\
        ({					\
                CAS32_TYPE res=0;		\
                ast_mutex_lock(_d);		\
                if (*_a == _b) {		\
                        res = _b;		\
                        *_a = _c;		\
                }				\
                ast_mutex_unlock(_d);		\
                res;				\
        })
#    define CAS_PTR(_a,_b,_c,_d) 		\
        ({					\
                boolean_t res = 0;		\
                ast_mutex_lock(_d);		\
                if (*_a == _b)	{		\
                        res = 1;		\
                        *_a = _c;		\
                }				\
                ast_mutex_unlock(_d);		\
                res;				\
        })
#  endif													/* SCCP_ATOMIC */

#endif														/* __SCCP_LOCK_H */
