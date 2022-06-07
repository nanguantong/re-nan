/**
 * @file lock.c  Pthread read/write locking
 *
 * Copyright (C) 2010 Creytiv.com
 */
#define _GNU_SOURCE 1
#include <pthread.h>
#include <re_types.h>
#include <re_mem.h>
#include <re_lock.h>


#define DEBUG_MODULE "lock"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


struct lock {
	pthread_rwlock_t lock;
};


static void lock_destructor(void *data)
{
	struct lock *l = data;

	int err = pthread_rwlock_destroy(&l->lock);
	if (err) {
		DEBUG_WARNING("pthread_rwlock_destroy: %m\n", err);
	}
}


/**
 * Allocate a new lock
 *
 * @param lp Pointer to allocated lock object
 *
 * @return 0 if success, otherwise errorcode
 */
int lock_alloc(struct lock **lp)
{
	struct lock *l;
	int err;

	if (!lp)
		return EINVAL;

	l = mem_zalloc(sizeof(*l), lock_destructor);
	if (!l)
		return ENOMEM;

	err = pthread_rwlock_init(&l->lock, NULL);
	if (err)
		goto out;

	*lp = l;

 out:
	if (err)
		mem_deref(l);
	return err;
}


/**
 * Get the lock for reading
 *
 * @param l Lock object
 */
void lock_read_get(struct lock *l)
{
	int err;

	if (!l)
		return;

	err = pthread_rwlock_rdlock(&l->lock);
	if (err) {
		DEBUG_WARNING("lock_read_get: %m\n", err);
	}
}


/**
 * Get the lock for writing
 *
 * @param l Lock object
 */
void lock_write_get(struct lock *l)
{
	int err;

	if (!l)
		return;

	err = pthread_rwlock_wrlock(&l->lock);
	if (err) {
		DEBUG_WARNING("lock_write_get: %m\n", err);
	}
}


/**
 * Attempt to get a lock for reading
 *
 * @param l Lock object
 *
 * @return 0 if success, otherwise errorcode
 */
int lock_read_try(struct lock *l)
{
	if (!l)
		return EINVAL;
	return pthread_rwlock_tryrdlock(&l->lock);
}


/**
 * Attempt to get a lock for writing
 *
 * @param l Lock object
 *
 * @return 0 if success, otherwise errorcode
 */
int lock_write_try(struct lock *l)
{
	if (!l)
		return EINVAL;
	return pthread_rwlock_trywrlock(&l->lock);
}


/**
 * Release a lock
 *
 * @param l Lock object
 */
void lock_rel(struct lock *l)
{
	int err;

	if (!l)
		return;

	err = pthread_rwlock_unlock(&l->lock);
	if (err) {
		DEBUG_WARNING("lock_rel: %m\n", err);
	}
}
