/* mountall
 *
 * Copyright © 2010 Canonical Ltd.
 * Author: Scott James Remnant <scott@netsplit.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */


#include <libudev.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mount.h>

#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <paths.h>
#include <stdio.h>
#include <limits.h>
#include <mntent.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <fnmatch.h>
#include <dirent.h>

#include <nih/macros.h>
#include <nih/alloc.h>
#include <nih/string.h>
#include <nih/list.h>
#include <nih/hash.h>
#include <nih/timer.h>
#include <nih/signal.h>
#include <nih/child.h>
#include <nih/io.h>
#include <nih/main.h>
#include <nih/option.h>
#include <nih/logging.h>
#include <nih/error.h>
#include <nih/errors.h>

#include <nih-dbus/dbus_error.h>
#include <nih-dbus/dbus_connection.h>
#include <nih-dbus/dbus_proxy.h>
#include <nih-dbus/errors.h>

#include <ply-event-loop.h>
#include <ply-boot-client.h>

#include "ioprio.h"

#include "dbus/upstart.h"
#include "com.ubuntu.Upstart.h"
#include "control.h"


#define BUILTIN_FSTAB   "/lib/init/fstab"

#define BOREDOM_TIMEOUT 3
#define ROOTDELAY 30

enum exit {
	EXIT_OK = 0,		/* Ok */
	EXIT_ERROR = 1,		/* General/OS error */
	EXIT_SHELL = 2,		/* Start maintenance shell */
	EXIT_SHELL_REBOOT = 3,	/* Start maintenance shell, reboot when done */
	EXIT_REBOOT = 4,	/* System must reboot */
};

typedef enum {
	TAG_UNKNOWN,
	TAG_LOCAL,
	TAG_REMOTE,
	TAG_VIRTUAL,
	TAG_SWAP,
	TAG_NOWAIT,
	TAG_SKIPPED,
	TAG_TIMEOUT,
} Tag;

typedef enum {
	ERROR_NONE,
	ERROR_BORED,
	ERROR_FSCK_FAILED,
	ERROR_FSCK_FAILED_HARD,
	ERROR_MOUNT_FAILED,
	ERROR_FSCK_IN_PROGRESS,
} Error;

typedef struct mount Mount;

struct mount {
	NihList             entry;

	char *              mountpoint;
	pid_t               mount_pid;
	int                 mounted;
	dev_t               mounted_dev;

	char *              device;
	struct udev_device *udev_device;
	NihHash *           physical_dev_ids;
	int                 physical_dev_ids_needed;
	pid_t               fsck_pid;
	int                 fsck_progress;
	int                 fsck_fix;
	int                 ready;

	DBusPendingCall *   pending_call;

	Error               error;

	char *              type;
	int                 nodev;
	char *              opts;
	char *              mount_opts;
	int                 check;

	Tag                 tag;
	int                 has_showthrough;
	Mount *             showthrough;
	int                 checked_link;
	Mount *             link_target;
	NihList             deps;
	int                 needs_mtab;
	char *              src;
};

#define MOUNT_NAME(_mnt) (strcmp ((_mnt)->type, "swap")			\
			  && strcmp ((_mnt)->type, "zfs")		\
			  && strcmp ((_mnt)->mountpoint, "none")	\
			  ? (_mnt)->mountpoint : (_mnt)->device)

typedef struct filesystem {
	char *name;
	int   nodev;
} Filesystem;

typedef struct process {
	Mount *       mnt;
	char * const *args;
	pid_t         pid;
	void (*handler) (Mount *mnt, pid_t pid, int status);
} Process;

typedef struct event_reply_data {
	Mount *          mnt;
	void (*handler) (void *data, NihDBusMessage *message);
} EventReplyData;

Mount *new_mount             (const char *mountpoint, const char *device,
			      int check, const char *type, const char *opts,
			      const char *src);
Mount *find_mount            (const char *mountpoint);
void   update_mount          (Mount *mnt, const char *device, int check,
			      const char *type, const char *opts);

int    has_option            (Mount *mnt, const char *option, int current);
char * get_option            (const void *parent, Mount *mnt, const char *option,
			      int current);
char * cut_options           (const void *parent, Mount *mnt, ...);

void   parse_fstab           (const char *filename);
void   parse_zfs_list        (void);
void   parse_mountinfo       (void);

void   parse_filesystems     (void);

void   mount_policy          (void);
void   tag_mount             (Mount *mnt, Tag tag);
void   mark_mounted          (void);

void   mounted               (Mount *mnt);
void   skip_mount            (Mount *mnt);
void   trigger_events        (void);

void   try_mounts            (void);
void   try_mount             (Mount *mnt, int force);
void   mounting_event_handled(void *data, NihDBusMessage *message);
void   mounted_event_handled (void *data, NihDBusMessage *message);

int    find_on_path          (const char *name);
int    find_fsck             (const char *type);

pid_t  spawn                 (Mount *mnt, char * const *args, int wait,
			      void (*handler) (Mount *mnt, pid_t pid, int status));
void   spawn_child_handler   (Process *proc, pid_t pid,
			      NihChildEvents event, int status);

void   run_mount             (Mount *mnt, int fake);
void   run_mount_finished    (Mount *mnt, pid_t pid, int status);

void   run_swapon            (Mount *mnt);
void   run_swapon_finished   (Mount *mnt, pid_t pid, int status);

int    missing_fsck          (Mount *mnt);
void   run_fsck              (Mount *mnt);
void   run_fsck_finished     (Mount *mnt, pid_t pid, int status);

void   write_mtab            (void);

void   mount_showthrough     (Mount *root);

void   upstart_disconnected  (DBusConnection *connection);
void   emit_event            (const char *name, Mount *mnt,
			      void (*cb) (void *, NihDBusMessage *message));
void   emit_event_error      (void *data, NihDBusMessage *message);

void   udev_monitor_watcher  (struct udev_monitor *udev_monitor,
			      NihIoWatch *watch, NihIoEvents events);
void   udev_catchup          (void);

void   fsck_update           (void);

void   fsck_reader           (Mount *mnt, NihIo *io,
			      const char *buf, size_t len);
void   boredom_timeout       (void *data, NihTimer *timer);

int    plymouth_connect      (void);
void   plymouth_disconnected (void *user_data, ply_boot_client_t *client);

void   plymouth_progress     (Mount *mnt, int progress);
void   plymouth_update       (int only_clear);

void   plymouth_response     (void *user_data, ply_boot_client_t *client);
void   plymouth_failed       (void *user_data, ply_boot_client_t *client);
void   plymouth_answer       (void *user_data, const char *keys,
			      ply_boot_client_t *client);

void   usr1_handler          (void *data, NihSignal *signal);
int    set_dev_wait_time        (NihOption *option, const char *arg);


/**
 * mounts:
 *
 * List of mounts that we need to process, parsed from /etc/fstab
 * (with built-ins and previously mounted filesystems added too).
 * Each list entry is a Mount structure.
 **/
NihList *mounts = NULL;

/**
 * Counters for the different tags.
 **/
size_t num_local = 0;
size_t num_local_mounted = 0;
size_t num_remote = 0;
size_t num_remote_mounted = 0;
size_t num_virtual = 0;
size_t num_virtual_mounted = 0;
size_t num_swap = 0;
size_t num_swap_mounted = 0;
size_t num_timeout = 0;
size_t num_timeout_mounted = 0;

/**
 * newly_mounted:
 *
 * Set to TRUE if we've successfully mounted something, informs the main loop
 * to try and mount non-mounted things again.
 **/
int newly_mounted = TRUE;

/**
 * fsck_in_progress:
 *
 * Set to TRUE while filesystem checks are in progress, this isn't quite the
 * same as fsck being running (which fsck_update) checks since fsck might
 * decide not to check the filesystems.
 *
 * Therefore this gets set to TRUE when we receive the first progress
 * information in fsck_progress(), and reset to FALSE in fsck_update() when
 * no fsck is running.
 **/
int fsck_in_progress = FALSE;

/**
 * filesystems:
 *
 * Array of filesystem information parsed from /proc/filesystems, primarily
 * used to eliminate arch-specific filesystems that don't exist when we
 * start and figure out which filesystems don't have devices.
 **/
Filesystem *filesystems = NULL;
size_t num_filesystems = 0;


/**
 * written_mtab:
 *
 * TRUE once we've successfully written /etc/mtab.
 **/
int written_mtab = FALSE;

/**
 * exit_code:
 *
 * Rather than exit immediately in case of error, we delay the exit until
 * any background processes (fsck, mount, etc.) have finished.
 **/
int exit_code = -1;


/**
 * upstart:
 *
 * Proxy to Upstart daemon.
 **/
static NihDBusProxy *upstart = NULL;

/**
 * udev:
 *
 * libudev context.
 **/
static struct udev *udev = NULL;

/**
 * ply_event_loop:
 *
 * plymouth event loop
 **/
static ply_event_loop_t *ply_event_loop = NULL;

/**
 * ply_boot_client:
 *
 * Plymouth boot client.
 **/
static ply_boot_client_t *ply_boot_client = NULL;

/**
 * plymouth_error:
 *
 * Error currently showing on Plymouth.
 **/
static Error plymouth_error = ERROR_NONE;

/**
 * plymouth_mnt:
 *
 * Mount point currently showing an error on Plymouth.
 **/
static Mount *plymouth_mnt = NULL;

/**
 * plymouth_keys:
 *
 * Keypresses currently being watched for from Plymouth.
 **/
static const char *plymouth_keys = NULL;

/**
 * boredom_timer:
 *
 * Having waited over BOREDOM_TIMEOUT seconds for certain mounts, print a
 * message to the user.
 **/
static NihTimer *boredom_timer = NULL;

/**
 * device_ready_timer:
 *
 * Having waited over ROOTDELAY seconds for certain devices to get ready,
 * exit mountall and trigger the upstart jobs for bringing up the device
 **/

NihTimer *device_ready_timer = NULL;

/**
 * daemonise:
 *
 * Set to TRUE if we should become a daemon, rather than just running
 * in the foreground.
 **/
static int daemonise = FALSE;

/**
 * force_fsck:
 *
 * Set to TRUE if we should pass -f to fsck invocations.
 **/
static int force_fsck = FALSE;

/**
 * fsck_fix:
 *
 * Set to TRUE if we should pass -y to fsck invocations.
 **/
static int fsck_fix = FALSE;

/**
 * no_events:
 *
 * Set to TRUE if we should not emit events.
 **/
static int no_events = FALSE;

/**
 ** dev-wait-time
 **
 ** Set to ROOTDELAY if not set. 
 ** Legal values are between 1sec to 2147483647 seconds.
 **/
static int dev_wait_time = FALSE;




static void
dequote (char *str)
{
	size_t len;

	nih_assert (str != NULL);

	if ((str[0] != '"') && (str[0] != '\''))
		return;

	len = strlen (str);
	len -= 2;

	memmove (str, str + 1, len);
	str[len] = '\0';
}

static void
strip_slashes (char *str)
{
	char *p;

	nih_assert (str != NULL);

	p = str + strlen (str) - 1;
	while ((p > str) && (*p == '/'))
		*(p--) = '\0';
}


Mount *
new_mount (const char *mountpoint,
	   const char *device,
	   int         check,
	   const char *type,
	   const char *opts,
	   const char *src)
{
	Mount *mnt;

	nih_assert (mountpoint != NULL);
	nih_assert (device != NULL);
	nih_assert (type != NULL);
	nih_assert (opts != NULL);

	mnt = NIH_MUST (nih_new (NULL, Mount));
	nih_list_init (&mnt->entry);

	mnt->mountpoint = NIH_MUST (nih_strdup (mounts, mountpoint));
	strip_slashes (mnt->mountpoint);
	mnt->checked_link = FALSE;
	mnt->link_target = NULL;

	mnt->mount_pid = -1;
	mnt->mounted = FALSE;
	mnt->mounted_dev = -1;

	mnt->device = NULL;
	mnt->udev_device = NULL;
	mnt->physical_dev_ids = NULL;
	mnt->physical_dev_ids_needed = TRUE;
	mnt->fsck_pid = -1;
	mnt->fsck_progress = -1;
	mnt->fsck_fix = FALSE;
	mnt->ready = FALSE;

	mnt->pending_call = NULL;

	mnt->error = ERROR_NONE;

	mnt->type = NULL;
	mnt->nodev = FALSE;
	mnt->opts = NULL;
	mnt->mount_opts = NULL;
	mnt->check = check;

	mnt->tag = TAG_UNKNOWN;
	mnt->has_showthrough = FALSE;
	mnt->showthrough = NULL;
	nih_list_init (&mnt->deps);

	nih_alloc_set_destructor (mnt, nih_list_destroy);
	nih_list_add (mounts, &mnt->entry);

	mnt->needs_mtab = FALSE;

	if (src)
		mnt->src = NIH_MUST (nih_strdup (mounts, src));
	update_mount (mnt, device, check, type, opts);

	return mnt;
}

Mount *
find_mount (const char *mountpoint)
{
	nih_assert (mountpoint != NULL);

	NIH_LIST_FOREACH (mounts, iter) {
		Mount *mnt = (Mount *)iter;

		if (! strcmp (mnt->mountpoint, mountpoint))
			return mnt;
	}

	return NULL;
}

void
update_mount (Mount *     mnt,
	      const char *device,
	      int         check,
	      const char *type,
	      const char *opts)
{
	nih_assert (mnt != NULL);

	if (device) {
		if (mnt->device)
			nih_unref (mnt->device, mounts);
		mnt->device = NIH_MUST (nih_strdup (mounts, device));

		if (! strncmp (mnt->device, "UUID=", 5)) {
			dequote (mnt->device + 5);
		} else if (! strncmp (mnt->device, "LABEL=", 6)) {
			dequote (mnt->device + 6);
		} else {
			char *colon;
			dequote (mnt->device);
			/* If our device name is in host:/path format, as is
			 * commonly used for network filesystems, don't strip
			 * trailing slashes if this is the entire path.  We
			 * look for the colon starting from the end, so that
			 * we correctly handle IPv6 addresses for the host
			 * part.
			 */
			if ((colon = strrchr (mnt->device,':')) != NULL
			    && colon[1] == '/')
				strip_slashes (colon + 2);
			else
				strip_slashes (mnt->device);
		}
	}

	if (check >= 0)
		mnt->check = check;

	if (type) {
		if (mnt->type)
			nih_unref (mnt->type, mounts);
		mnt->type = NIH_MUST (nih_strdup (mounts, type));
	}

	if (opts) {
		if (mnt->opts)
			nih_unref (mnt->opts, mounts);
		mnt->opts = NIH_MUST (nih_strdup (mounts, opts));
	}


	nih_debug ("%s: %s %s %s %s%s",
		   MOUNT_NAME (mnt),
		   mnt->mountpoint,
		   mnt->device,
		   mnt->type,
		   mnt->opts,
		   mnt->check ? " check" : "");
}


int
has_option (Mount *     mnt,
	    const char *option,
	    int         current)
{
	const char *opts;
	size_t      i;

	nih_assert (mnt != NULL);
	nih_assert (option != NULL);

	opts = current ? mnt->mount_opts : mnt->opts;
	if (! opts)
		return FALSE;

	i = 0;
	while (i < strlen (opts)) {
		size_t j;
		size_t k;

		j = strcspn (opts + i, ",=");
		k = strcspn (opts + i + j, ",");

		if (! strncmp (opts + i, option, j))
			return TRUE;

		i += j + k + 1;
	}

	return FALSE;
}

char *
get_option (const void *parent,
	    Mount *     mnt,
	    const char *option,
	    int         current)
{
	const char *opts;
	size_t      i;

	nih_assert (mnt != NULL);
	nih_assert (option != NULL);

	opts = current ? mnt->mount_opts : mnt->opts;
	if (! opts)
		return NULL;

	i = 0;
	while (i < strlen (opts)) {
		size_t j;
		size_t k;

		j = strcspn (opts + i, ",=");
		k = strcspn (opts + i + j, ",");

		if (! strncmp (opts + i, option, j))
			return nih_strndup (parent, opts + i + j + 1,
					    k ? k - 1 : 0);

		i += j + k + 1;
	}

	return NULL;
}

char *
cut_options (const void *parent,
	     Mount *     mnt,
	     ...)
{
	va_list args;
	char *  opts;
	size_t  i;

	nih_assert (mnt != NULL);

	va_start (args, mnt);

	opts = NIH_MUST (nih_strdup (parent, mnt->opts));

	i = 0;
	while (i < strlen (opts)) {
		size_t      j;
		size_t      k;
		va_list     options;
		const char *option;

		j = strcspn (opts + i, ",=");
		k = strcspn (opts + i + j, ",");

		va_copy (options, args);
		while ((option = va_arg (options, const char *)) != NULL) {
			if (j && strlen(option) == j
				&& ! strncmp (opts + i, option, j))
				break;
		}
		va_end (options);

		if (option) {
			memmove (opts + (i ? i - 1 : 0), opts + i + j + k,
				 strlen (opts) - i - j - k + 1);
		} else {
			i += j + k + 1;
		}
	}

	va_end (args);

	return opts;
}


void
parse_fstab (const char *filename)
{
	FILE *         fstab;
	struct mntent *mntent;

	nih_assert (filename != NULL);

	nih_debug ("updating from %s", filename);

	fstab = setmntent (filename, "r");
	if (! fstab) {
		nih_fatal ("%s: %s", filename, strerror (errno));
		exit (EXIT_ERROR);
	}

	while ((mntent = getmntent (fstab)) != NULL) {
		Mount *         mnt;
		nih_local char *fsname = NULL;

		/* These are always symlinks into /run now.  Ignore any
		 * explicit attempts to mount them because it vastly
		 * complicates matters when trying to ensure that /run and
		 * /var/run point to the same place.
		 */
		if (! strcmp (mntent->mnt_dir, "/var/run") ||
		    ! strcmp (mntent->mnt_dir, "/var/lock"))
			continue;

		mnt = find_mount (mntent->mnt_dir);
		if (mnt && (!mnt->src || strcmp (mnt->src, filename))
		    && strcmp (mntent->mnt_type, "swap")) {
			update_mount (mnt,
				      mntent->mnt_fsname,
				      mntent->mnt_passno != 0,
				      mntent->mnt_type,
				      mntent->mnt_opts);
		} else {
			new_mount (mntent->mnt_dir,
				   mntent->mnt_fsname,
				   mntent->mnt_passno != 0,
				   mntent->mnt_type,
				   mntent->mnt_opts,
				   filename);
		}
	}

	endmntent (fstab);
}

/**
 * parse_zfs_list:
 *
 * Add the output of `/sbin/zfs list` to the internal mounts list
 * and pretend that these filesystems were in the /etc/fstab file.
 *
 * @TODO: Check whether Canonical agrees that libzfs, which is
 * subject to the CDDL, should get the GPL system library exemption
 * like it usually does on BSD and Solaris systems.
 *
 * @TODO: Subtitute the call to popen with a libzfs routine.
 *
 * @TODO: Double-check whether the new src parameter should be NULL
 * or set to a dummy fstab filename.
 *
 **/
void
parse_zfs_list (void)
{
	nih_local char *buf = NULL;
	size_t bufsz = 4096;
	FILE *zfs_stream;
	const char zfs_command[] =
	  "/sbin/zfs list -H -t filesystem -o name,mountpoint,canmount,readonly,atime";
	int zfs_result;

	nih_debug ("parsing ZFS list");

	fflush (stdout);
	fflush (stderr);
	zfs_stream = popen (zfs_command, "r");

	if (zfs_stream == NULL) {
		nih_debug ("popen /sbin/zfs: %s", strerror (errno));
		return;
	}

	buf = NIH_MUST (nih_alloc (NULL, bufsz));

	/* Read one line from the pipe into the buffer. */
	while (fgets (buf, bufsz, zfs_stream) != NULL) {
		char *saveptr;
		char *zfs_name, *zfs_mountpoint, *zfs_canmount, *zfs_optatime, *zfs_optronly;
		nih_local char *zfs_mntoptions = NULL;

		/* If the line didn't fit, then enlarge the buffer and retry. */
		while ((! strchr (buf, '\n')) && (! feof (zfs_stream))) {
			buf = NIH_MUST (nih_realloc (buf, NULL, bufsz + 4096));
			if (! fgets (buf + bufsz - 1, 4097, zfs_stream)) {
				break;
			}
			bufsz += 4096;
		}

		zfs_name = strtok_r (buf, "\t", &saveptr);
		if (! zfs_name) {
			continue;
		}

		/* ASSERT: mountpoint[0] = '/' OR mountpoint = none | legacy */
		zfs_mountpoint = strtok_r (NULL, "\t", &saveptr);
		if (! zfs_mountpoint || zfs_mountpoint[0] != '/') {
			continue;
		}

		/* ASSERT: canmount = on | off | noauto */
		zfs_canmount = strtok_r (NULL, "\t", &saveptr);
		if (! zfs_canmount || strcmp (zfs_canmount, "on")) {
			continue;
		}

		NIH_MUST (nih_strcat (&zfs_mntoptions, NULL, "zfsutil"));

		/* ASSERT: readonly = on | off */
		zfs_optronly = strtok_r (NULL, "\t", &saveptr);
		if ( zfs_optronly && strcmp (zfs_optronly, "off") ) {
			NIH_MUST (nih_strcat (&zfs_mntoptions, NULL, ",ro"));
		}

		/* ASSERT: atime = on | off */
		zfs_optatime = strtok_r (NULL, "\t\n", &saveptr);
		if ( zfs_optatime && strcmp (zfs_optatime, "on") ) {
			NIH_MUST (nih_strcat (&zfs_mntoptions, NULL, ",noatime"));
		}

		new_mount (zfs_mountpoint, zfs_name, FALSE, "zfs", zfs_mntoptions, NULL);
	}

	zfs_result = pclose (zfs_stream);

	if (zfs_result)
	{
		nih_debug ("pclose /sbin/zfs: %i %s", zfs_result, strerror (errno));
	}
}

static int
needs_remount (Mount *mnt)
{
	nih_assert (mnt != NULL);

	if (mnt->mounted
	    && has_option (mnt, "ro", TRUE)
	    && (! has_option (mnt, "ro", FALSE))) {
		return TRUE;
	} else {
		return FALSE;
	}
}

static void
mount_proc (void)
{
	Mount *mnt;

	nih_debug ("mounting /proc");
	if (mount ("none", "/proc", "proc",
		   MS_NODEV | MS_NOEXEC | MS_NOSUID, NULL) < 0) {
		nih_fatal ("%s: %s: %s", "/proc",
			   _("unable to mount"),
			   strerror (errno));
		exit (EXIT_ERROR);
	}

	mnt = find_mount ("/proc");
	if (mnt)
		mnt->mounted = TRUE;
}

static FILE *mountinfo = NULL;

static void
parse_mountinfo_file (int reparsed)
{
	nih_local char *buf = NULL;
	size_t          bufsz;

	nih_assert (mountinfo != NULL);

	if (reparsed)
		rewind (mountinfo);

	nih_debug ("updating mounts");

	bufsz = 4096;
	buf = NIH_MUST (nih_alloc (NULL, bufsz));

	while (fgets (buf, bufsz, mountinfo) != NULL) {
		char * saveptr;
		char * ptr;
		char * dev;
		char * mountpoint;
		char * type;
		char * device;
		int    maj, min;
		char * mount_opts;
		char * super_opts;
		char * opts;
		Mount *mnt;

		while ((! strchr (buf, '\n')) && (! feof (mountinfo))) {
			buf = NIH_MUST (nih_realloc (buf, NULL, bufsz + 4096));
			if (! fgets (buf + bufsz - 1, 4097, mountinfo))
				break;
			bufsz += 4096;
		}

		/* mount ID */
		ptr = strtok_r (buf, " \t\n", &saveptr);
		if (! ptr)
			continue;

		/* parent ID */
		ptr = strtok_r (NULL, " \t\n", &saveptr);
		if (! ptr)
			continue;

		/* major:minor */
		dev = strtok_r (NULL, " \t\n", &saveptr);
		if (! dev)
			continue;

		/* root */
		ptr = strtok_r (NULL, " \t\n", &saveptr);
		if (! ptr)
			continue;

		/* mountpoint */
		mountpoint = strtok_r (NULL, " \t\n", &saveptr);
		if (! mountpoint)
			continue;

		/* mount opts */
		mount_opts = strtok_r (NULL, " \t\n", &saveptr);
		if (! mount_opts)
			continue;

		/* optional fields */
		while (((ptr = strtok_r (NULL, " \t\n", &saveptr)) != NULL)
		       && strcmp (ptr, "-"))
			;
		if (! ptr)
			continue;

		/* type */
		type = strtok_r (NULL, " \t\n", &saveptr);
		if (! type)
			continue;

		/* device */
		device = strtok_r (NULL, " \t\n", &saveptr);
		if (! device)
			continue;

		/* superblock opts */
		super_opts = strtok_r (NULL, " \t\n", &saveptr);
		if (! super_opts)
			continue;

		/* ignore ureadahead's mounting of debugfs */
		if (! strcmp (mountpoint, "/var/lib/ureadahead/debugfs"))
			continue;

		opts = NIH_MUST (nih_sprintf (mounts, "%s,%s",
					      mount_opts, super_opts));

		mnt = find_mount (mountpoint);
		if (mnt
		    && strcmp (type, "swap")) {

			int needed_remount = needs_remount (mnt);

			if (! strcmp (type, "rootfs"))
				type = NULL;

			if ((! strcmp (device, "/dev/root"))
			    || (! strcmp (device, "none")))
				device = NULL;

			update_mount (mnt, device, -1, type, NULL);

			if (mnt->mount_opts)
				nih_unref (mnt->mount_opts, mounts);
			mnt->mount_opts = opts;

			/* If remounted, treat as newly done mount below. */
			if (needed_remount && !needs_remount (mnt))
				mnt->mounted = FALSE;

		} else {
			mnt = new_mount (mountpoint, device, FALSE, type, opts,
			                 NULL);
			mnt->mount_opts = opts;
		}

		if (sscanf (dev, "%d:%d", &maj, &min) == 2)
			mnt->mounted_dev = makedev (maj, min);

		if (mnt->mounted)
			continue;

		/* Fully update mounted state if reparsed.
		 * If invoked first time, just set the flag and leave complete
		 * state update (including event trigger) to mark_mounted ().
		 */
		if (reparsed && ! mnt->pending_call) {
			mounted (mnt);
		} else {
			mnt->mounted = TRUE;
		}
	}
}

static void
mountinfo_watcher (void *      data,
		   NihIoWatch *watch,
		   NihIoEvents events)
{
	nih_assert (mountinfo != NULL);

	nih_debug ("mountinfo changed, reparsing");
	parse_mountinfo_file (TRUE);
}

void
parse_mountinfo (void)
{
	if (mountinfo) {
		parse_mountinfo_file (TRUE);
	} else {
		mountinfo = fopen ("/proc/self/mountinfo", "r");
		if ((! mountinfo) && (errno == ENOENT)) {
			mount_proc ();
			mountinfo = fopen ("/proc/self/mountinfo", "r");
		}
		if (! mountinfo) {
			nih_fatal ("%s: %s", "/proc/self/mountinfo",
				   strerror (errno));
			exit (EXIT_ERROR);
		}

		parse_mountinfo_file (FALSE);

		NIH_MUST (nih_io_add_watch (NULL, fileno (mountinfo), NIH_IO_EXCEPT,
					    mountinfo_watcher, NULL));
	}
}


void
parse_filesystems (void)
{
	FILE *          fs;
	nih_local char *buf = NULL;
	size_t          bufsz;

	nih_debug ("reading filesystems");

	fs = fopen ("/proc/filesystems", "r");
	if ((! fs) && (errno == ENOENT)) {
		mount_proc ();
		fs = fopen ("/proc/filesystems", "r");
	}
	if (! fs) {
		nih_fatal ("%s: %s", "/proc/filesystems",
			   strerror (errno));
		exit (EXIT_ERROR);
	}

	bufsz = 4096;
	buf = NIH_MUST (nih_alloc (NULL, bufsz));

	while (fgets (buf, bufsz, fs) != NULL) {
		char *      ptr;
		int         nodev;
		Filesystem *filesystem;

		while (((ptr = strchr (buf, '\n')) == NULL) && (! feof (fs))) {
			buf = NIH_MUST (nih_realloc (buf, NULL, bufsz + 4096));
			if (! fgets (buf + bufsz - 1, 4097, fs))
				break;
			bufsz += 4096;
		}

		*ptr = '\0';
		if (! strncmp (buf, "nodev\t", 6)) {
			nodev = TRUE;
			ptr = buf + 6;
			nih_debug ("%s (nodev)", ptr);
		} else if (buf[0] == '\t') {
			nodev = FALSE;
			ptr = buf + 1;
			nih_debug ("%s", ptr);
		} else
			continue;

		filesystems = NIH_MUST (nih_realloc (filesystems, NULL,
						     sizeof (Filesystem) * (num_filesystems + 1)));
		filesystem = &filesystems[num_filesystems++];

		filesystem->name = NIH_MUST (nih_strdup (filesystems, ptr));
		filesystem->nodev = nodev;
	}

	if (fclose (fs) < 0) {
		nih_fatal ("%s: %s", "/proc/filesystems",
			  strerror (errno));
		exit (EXIT_ERROR);
	}
}


static int
is_parent (char *root,
	   char *path)
{
	size_t len;

	nih_assert (root != NULL);
	nih_assert (path != NULL);

	len = strlen (root);
	if ((! strncmp (path, root, len))
	    && ((path[len] == '/') || (len && path[len-1] == '/')))
		return TRUE;

	return FALSE;
}

static int
is_remote (Mount *mnt)
{
	nih_assert (mnt != NULL);

	if (has_option (mnt, "_netdev", FALSE)
	    || (! strcmp (mnt->type, "nfs"))
	    || (! strcmp (mnt->type, "nfs4"))
	    || (! strcmp (mnt->type, "smbfs"))
	    || (! strcmp (mnt->type, "cifs"))
	    || (! strcmp (mnt->type, "coda"))
	    || (! strcmp (mnt->type, "ncp"))
	    || (! strcmp (mnt->type, "ncpfs"))
	    || (! strcmp (mnt->type, "ocfs2"))
	    || (! strcmp (mnt->type, "gfs"))
	    || (! strcmp (mnt->type, "gfs2"))
	    || (! strcmp (mnt->type, "ceph"))) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void
mount_policy (void)
{
	nih_local NihList  *already_mounted = NULL;
	NihListEntry       *entry;

	/* Build a list of mountpoints of already mounted
	 * file systems. Used later to avoid hiding existing
	 * mounted file systems.
	 */
	already_mounted = NIH_MUST (nih_list_new (NULL));
 	NIH_LIST_FOREACH (mounts, iter) {
		Mount *mnt = (Mount *)iter;
		if (mnt->mountpoint && mnt->mounted) {
			entry = NIH_MUST (nih_list_entry_new (already_mounted));
			entry->str = NIH_MUST (nih_strdup (entry, mnt->mountpoint));
			nih_list_add (already_mounted, &entry->entry);
		}
	}

 	NIH_LIST_FOREACH_SAFE (mounts, iter) {
		Mount *mnt = (Mount *)iter;
		size_t j;

		/* Check through the known filesystems, if this is a nodev
		 * filesystem then mark the mount as such so we don't wait
		 * for any device to be ready.
		 */
		for (j = 0; j < num_filesystems; j++) {
			if ((! strcmp (mnt->type, filesystems[j].name))
			    && filesystems[j].nodev) {
				mnt->nodev = TRUE;
				break;
			}
		}

		/* If there's no device spec for this filesystem,
		 * and it's optional, we ignore it.
		 */
		if ((! strcmp (mnt->device, "none"))
		    && has_option (mnt, "optional", FALSE)
		    && (j == num_filesystems)) {
			nih_debug ("%s: dropping unknown filesystem",
				   MOUNT_NAME (mnt));
			nih_free (mnt);
			continue;
		}

		/* Otherwise if there's no device and the type is also "none",
		 * special-case it as a "nodev" virtual filesystem so our
		 * placeholder handling kicks in.
		 */
		if (! strcmp (mnt->device, "none")
		    && ! strcmp (mnt->type, "none"))
			mnt->nodev = TRUE;

		/* Drop anything with ignore as its type. */
		if (! strcmp (mnt->type, "ignore")) {
			nih_debug ("%s: dropping ignored filesystem",
				   MOUNT_NAME (mnt));
			nih_free (mnt);
			continue;
		}

		/* Drop anything that's not auto-mounted which isn't already
		 * mounted.
		 */
		if (has_option (mnt, "noauto", FALSE)
		    && (! mnt->mounted)) {
			nih_debug ("%s: dropping noauto filesystem",
				   MOUNT_NAME (mnt));
			nih_free (mnt);
			continue;
		}

		/* Check if our current mount entry would hide
		 * an existing mountpoint. If so, skip it.
		 */
		NIH_LIST_FOREACH (already_mounted, iter) {
			NihListEntry *e = (NihListEntry *)iter;
			if (mnt->mountpoint
			    && mnt->mountpoint != e->str
			    && ! mnt->mounted
			    && is_parent (mnt->mountpoint, e->str)) {
				nih_debug (
				    "%s: dropping filesystem because it has an already mounted child (%s)\n",
				    mnt->mountpoint, e->str);
				nih_free (mnt);
				break;
			}
		}
	}

	for (NihList *iter = mounts->prev; iter != mounts; iter = iter->prev) {
		Mount *mnt = (Mount *)iter;
		Mount *mount_parent = NULL;
		Mount *device_parent = NULL;

		/* Iterate through the list of mounts, we're looking for the
		 * parentof the mountpoint (e.g. /var/tmp's parent is /var)
		 * and the parent mountpoint of the device spec
		 * (e.g. /usr/loop.img's parent is /usr).
		 */
		NIH_LIST_FOREACH (mounts, iter) {
			Mount *other = (Mount *)iter;
			int is_dep = 0;

			/* Skip this mount entry */
			if (other == mnt)
				continue;

			/* Since multiple mounts can have the same mount point,
			 * we check here if the target already has us as a
			 * dep to avoid a circular dependency. */
			NIH_LIST_FOREACH (&(other->deps), iter) {
				NihListEntry *dep = (NihListEntry *)iter;
				if (dep->data == mnt)
					is_dep = 1;
			}
			if (is_dep)
				continue;

			/* Is this a parent of our mountpoint? */
			if (mnt->mountpoint
			    && other->mountpoint
			    && is_parent (other->mountpoint, mnt->mountpoint)){
				if ((! mount_parent)
				    || (strlen (mount_parent->mountpoint) < strlen (other->mountpoint)))
					mount_parent = other;
			}

			/* Is this a parent mountpoint of our device? */
			if ((! mnt->nodev)
			    && other->mountpoint
			    && is_parent (other->mountpoint, mnt->device)){
				if ((! device_parent)
				    || (strlen (device_parent->mountpoint) < strlen (other->mountpoint)))
					device_parent = other;
			}
		}

		/* We nearly always depend on our mountpoint's parent, the only
		 * exceptions are swap devices (which aren't mounted),
		 * showthrough entries (which explicitly don't need to wait)
		 * and virtual filesystems directly under the root.
		 */
		if (mount_parent
		    && strcmp (mnt->type, "swap")) {
			if (has_option (mnt, "showthrough", FALSE)
			    && strcmp (mount_parent->mountpoint, "/")) {
				mount_parent->has_showthrough = TRUE;
				mnt->showthrough = mount_parent;
				nih_debug ("%s shows through parent %s",
					   MOUNT_NAME (mnt),
					   MOUNT_NAME (mount_parent));
				mount_parent = NULL;
			} else if ((! strcmp (mount_parent->mountpoint, "/"))
				   && mnt->nodev
				   && strcmp (mnt->type, "none")) {
				nih_debug ("%s can be mounted while root readonly",
					   MOUNT_NAME (mnt));
				mount_parent = NULL;
			} else {
				NihListEntry *dep;

				dep = NIH_MUST (nih_list_entry_new (mnt));
				dep->data = mount_parent;
				nih_ref (mount_parent, dep);

				nih_list_add (&mnt->deps, &dep->entry);
				nih_debug ("%s parent is %s",
					   MOUNT_NAME (mnt),
					   MOUNT_NAME (mount_parent));
			}
		}

		/* It's also a pretty good idea to wait for the mountpoint on
		 * which our device exists to appear first, assuming we have
		 * one and that it's a valid path.
		 */
		if (device_parent
		    && (! mnt->nodev)
		    && (mnt->device[0] == '/')
		    && strncmp (mnt->device, "/dev/", 5)) {
			NihListEntry *dep;

			dep = NIH_MUST (nih_list_entry_new (mnt));
			dep->data = device_parent;
			nih_ref (device_parent, dep);

			nih_list_add (&mnt->deps, &dep->entry);
			nih_debug ("%s parent is %s (mount %s)",
				   MOUNT_NAME (mnt),
				   MOUNT_NAME (device_parent),
				   mnt->device);
		}

		/* Kernel filesystems can require rather more to mount than
		 * first appears, if a device spec has been given then we
		 * also wait for the previous fstab entry - whatever it was.
		 */
		if (mnt->nodev
		    && strcmp (mnt->device, "none")
		    && strcmp (mnt->device, mnt->type)
		    && (mnt->entry.prev != mounts)
		    && strcmp (((Mount *)mnt->entry.prev)->device, "none")
		    && strcmp (((Mount *)mnt->entry.prev)->type, "swap")) {
			NihListEntry *dep;
			Mount *       prior;

			prior = (Mount *)mnt->entry.prev;

			dep = NIH_MUST (nih_list_entry_new (mnt));
			dep->data = prior;
			nih_ref (prior, dep);

			nih_list_add (&mnt->deps, &dep->entry);
			nih_debug ("%s prior fstab entry %s",
				   MOUNT_NAME (mnt), MOUNT_NAME (prior));
		}

		/* Tag the filesystem so we know which event it blocks */
		tag_mount (mnt, TAG_UNKNOWN);
	}

	NIH_LIST_FOREACH (mounts, iter) {
		Mount *mnt = (Mount *)iter;

		switch (mnt->tag) {
		case TAG_VIRTUAL:
			nih_info (_("%s is %s"), MOUNT_NAME (mnt), "virtual");
			num_virtual++;
			break;
		case TAG_LOCAL:
			nih_info (_("%s is %s"), MOUNT_NAME (mnt), "local");
			num_local++;
			break;
		case TAG_TIMEOUT:
			nih_info (_("%s is %s"), MOUNT_NAME (mnt), "local with timeout");
			num_local++;
			num_timeout++;
			break;
		case TAG_REMOTE:
			nih_info (_("%s is %s"), MOUNT_NAME (mnt), "remote");
			num_remote++;
			break;
		case TAG_SWAP:
			nih_info (_("%s is %s"), MOUNT_NAME (mnt), "swap");
			num_swap++;
			break;
		case TAG_NOWAIT:
			nih_info (_("%s is %s"), MOUNT_NAME (mnt), "nowait");
			break;
		case TAG_SKIPPED:
			nih_info (_("%s is %s"), MOUNT_NAME (mnt), "skipped");
			break;
		default:
			nih_assert_not_reached ();
		}
	}
}

void
tag_mount (Mount *mnt,
	   Tag    tag)
{
	Tag initial_tag = mnt->tag;

	nih_assert (mnt != NULL);

	switch (tag) {
	case TAG_UNKNOWN:
	case TAG_LOCAL:
	case TAG_REMOTE:
	case TAG_NOWAIT:
		break;
	default:
		/* Not an inheritable tag. */
		nih_assert_not_reached ();
	}

	/* First time we're asked about this mount, figure out its
	 * intrinsic tag from the device type */
	if (mnt->tag == TAG_UNKNOWN) {
		if (! strcmp (mnt->type, "swap")) {
			mnt->tag = TAG_SWAP;
		} else if (! strcmp (mnt->mountpoint, "/")) {
			nih_debug ("%s is root filesystem", MOUNT_NAME (mnt));
			mnt->tag = TAG_LOCAL;
		} else if (is_remote (mnt)) {
			if (((! strcmp (mnt->mountpoint, "/usr"))
			     || (! strcmp (mnt->mountpoint, "/var"))
			     || (! strncmp (mnt->mountpoint, "/usr/", 5))
			     || (! strncmp (mnt->mountpoint, "/var/", 5))
			     || (has_option (mnt, "bootwait", FALSE)))
			    && ! has_option (mnt, "nobootwait", FALSE))
			{
				mnt->tag = TAG_REMOTE;
			} else {
				mnt->tag = TAG_NOWAIT;
			}
		} else if (mnt->nodev
			   && strcmp (mnt->type, "fuse")) {
			if (! has_option (mnt, "nobootwait", FALSE)) {
				mnt->tag = TAG_VIRTUAL;
			} else {
				mnt->tag = TAG_NOWAIT;
			}
		} else {
			if (! has_option (mnt, "nobootwait", FALSE)) {
				if ( has_option (mnt, "timeout", FALSE))
				{
					mnt->tag = TAG_TIMEOUT; 
				}
				else
					mnt->tag = TAG_LOCAL;

			} else {
				mnt->tag = TAG_NOWAIT;
			}
		}
	}

	/* If no tag is passed, default it from our dependencies */
	if (tag == TAG_UNKNOWN) {
		NIH_LIST_FOREACH (&mnt->deps, dep_iter) {
			NihListEntry *dep_entry = (NihListEntry *)dep_iter;
			Mount *       dep = (Mount *)dep_entry->data;

			/* Do not inherit from the root filesystem unless
			 * this is a placeholder.
			 */
			if ((! strcmp (dep->mountpoint, "/"))
			    && strcmp (mnt->type, "none"))
				continue;

			if (dep->tag == TAG_NOWAIT)
				tag = TAG_NOWAIT;
			if ((dep->tag == TAG_REMOTE)
			    && (tag != TAG_NOWAIT))
				tag = TAG_REMOTE;
			if (((dep->tag == TAG_LOCAL)
			     || (dep->tag == TAG_TIMEOUT))
			    && (tag != TAG_REMOTE)
			    && (tag != TAG_NOWAIT))
				tag = TAG_LOCAL;
		}
	}

	/* Don't override a more restrictive tag already set. */
	if (mnt->tag == TAG_NOWAIT)
		tag = TAG_NOWAIT;
	if ((mnt->tag == TAG_REMOTE)
	    && (tag != TAG_NOWAIT))
		tag = TAG_REMOTE;
	if (((mnt->tag == TAG_LOCAL)
	     || (mnt->tag == TAG_TIMEOUT))
	    && (tag != TAG_REMOTE)
	    && (tag != TAG_NOWAIT))
		tag = TAG_LOCAL;

	/* Set the tag, then set it on any mount point that depends on this
	 * one. */
	if (tag != TAG_UNKNOWN)
		mnt->tag = tag;

	tag = mnt->tag;

	/* TAG_TIMEOUT is TAG_LOCAL with a timeout. timeout cannot be
	 * inherited but local could be */
	if (tag == TAG_TIMEOUT)
		tag = TAG_LOCAL;
	if (initial_tag == TAG_TIMEOUT)
		initial_tag = TAG_LOCAL;

	/* Not inheritable, don't recurse */
	if ((tag != TAG_LOCAL)
	    && (tag != TAG_REMOTE)
	    && (tag != TAG_NOWAIT))
		return;

	/* No changes, so don't recurse. */
	if (tag == initial_tag)
		return;

	NIH_LIST_FOREACH (mounts, iter) {
		Mount *dep = (Mount *)iter;

		/* Do not inherit from the root filesystem unless
		 * this is a placeholder.
		 */
		if ((! strcmp (mnt->mountpoint, "/"))
		    && strcmp (dep->type, "none"))
			continue;

		NIH_LIST_FOREACH (&dep->deps, dep_iter) {
			NihListEntry *dep_entry = (NihListEntry *)dep_iter;

			if (dep_entry->data == mnt)
				tag_mount (dep, tag);
		}
	}
}

void
mark_mounted (void)
{
 	NIH_LIST_FOREACH (mounts, iter) {
		Mount *mnt = (Mount *)iter;

		if (mnt->mounted && (! needs_remount (mnt)))
			mounted (mnt);
	}
}

void
mounted (Mount *mnt)
{
	nih_assert (mnt != NULL);

	nih_debug ("%s", MOUNT_NAME (mnt));

	mnt->error = ERROR_NONE;
	plymouth_update (FALSE);

	/* This is a completely special case, because we just broke the
	 * system in a way that it's not really going to expect.
	 */
	if (! strcmp (mnt->mountpoint, "/dev")) {
		mode_t mask;
		Mount *root;

		mask = umask (0000);
		mknod ("/dev/console", S_IFCHR | 0600, makedev (5, 1));
		mknod ("/dev/null", S_IFCHR | 0666, makedev (1, 3));
		umask (mask);

		/* And while we're in here, let's make sure we get a
		 * /dev/root symlink for the right device too ;-)
		 */
		root = find_mount ("/");
		if (root && root->mounted_dev != (dev_t)-1) {
			FILE *rules;

			mask = umask (0022);
			mkdir ("/dev/.udev", 0755);
			mkdir ("/dev/.udev/rules.d", 0755);
			rules = fopen ("/dev/.udev/rules.d/root.rules", "w");
			if (rules) {
				fprintf (rules, ("SUBSYSTEM==\"block\", "
						 "ENV{MAJOR}==\"%d\", "
						 "ENV{MINOR}==\"%d\", "
						 "SYMLINK+=\"root\"\n"),
					 major (root->mounted_dev),
					 minor (root->mounted_dev));
				fclose (rules);
			}
			umask (mask);
		}
	}

	if (!mnt->pending_call)
		emit_event ("mounted", mnt, mounted_event_handled);

	fsck_update ();
}

void
skip_mount (Mount *mnt)
{
	nih_assert (mnt != NULL);
	if (mnt->mounted && ! needs_remount (mnt))
		return;

	nih_debug ("%s", MOUNT_NAME (mnt));

	switch (mnt->tag) {
	case TAG_LOCAL:
		num_local--;
		break;
	case TAG_TIMEOUT:
		num_local--;
		num_timeout--;
		break;
	case TAG_REMOTE:
		num_remote--;
		break;
	case TAG_VIRTUAL:
		num_virtual--;
		break;
	case TAG_SWAP:
		num_swap--;
		break;
	case TAG_NOWAIT:
		break;
	case TAG_SKIPPED:
		break;
	case TAG_UNKNOWN:
		break;
	default:
		nih_assert_not_reached ();
	}

	mnt->tag = TAG_SKIPPED;
	newly_mounted = TRUE;

	NIH_LIST_FOREACH (mounts, iter) {
		Mount *dep = (Mount *)iter;

		if (dep->mounted && (! needs_remount (dep)))
			continue;

		NIH_LIST_FOREACH (&dep->deps, dep_iter) {
			NihListEntry *dep_entry = (NihListEntry *)dep_iter;

			if (dep_entry->data == mnt)
				skip_mount (dep);
		}
	}
	
	trigger_events ();
}

void
trigger_events (void)
{
	static int virtual_triggered = FALSE;
	static int local_triggered = FALSE;
	static int remote_triggered = FALSE;
	static int swap_triggered = FALSE;
	static int filesystem_triggered = FALSE;

	/* Virtual may be triggered at any time */
	if ((! virtual_triggered)
	    && (num_virtual_mounted == num_virtual)) {
		nih_info (_("%s finished"), "virtual");
		emit_event ("virtual-filesystems", NULL, NULL);
		virtual_triggered = TRUE;
	}

	/* Enforce local only after virtual filesystems triggered */
	if ((! local_triggered)
	    && virtual_triggered
	    && (num_local_mounted == num_local)) {
		nih_info (_("%s finished"), "local");
		emit_event ("local-filesystems", NULL, NULL);
		local_triggered = TRUE;
	}

	/* Enforce remote only after virtual filesystems triggrered */
	if ((! remote_triggered)
	    && virtual_triggered
	    && (num_remote_mounted == num_remote)) {
		nih_info (_("%s finished"), "remote");
		emit_event ("remote-filesystems", NULL, NULL);
		remote_triggered = TRUE;
	}

	/* Aggregate event for all filesystems */
	if ((! filesystem_triggered)
	    && virtual_triggered
	    && local_triggered
	    && remote_triggered) {
		nih_info (_("All filesystems mounted"));
		emit_event ("filesystem", NULL, NULL);
		filesystem_triggered = TRUE;
	}

	/* Swaps may be triggered at any time */
	if ((! swap_triggered)
	    && (num_swap_mounted == num_swap)) {
		nih_info (_("%s finished"), "swap");
		emit_event ("all-swaps", NULL, NULL);
		swap_triggered = TRUE;
	}

	nih_info ("local %zi/%zi remote %zi/%zi virtual %zi/%zi swap %zi/%zi",
		   num_local_mounted, num_local,
		   num_remote_mounted, num_remote,
		   num_virtual_mounted, num_virtual,
		   num_swap_mounted, num_swap);
}

/* 
 * is_device_ready:
 *
 * @data: Not used
 * @timer: timer which was preconfigured and on the expiry of which this
 * function is called.
 *
 * This function is called to check if any device marked with the "timeout"
 * option in the /etc/fstab is not ready when the preconfigured @timer
 * expires.
 * It emits a "device-not-ready" event when it finds that such a device is not
 * yet ready.
 */
void
is_device_ready (void * data,
		 NihTimer *timer)
{
	device_ready_timer = NULL;
	NIH_LIST_FOREACH (mounts, iter) {
		Mount *mnt = (Mount *)iter;

		if (mnt->mounted)
			continue;
		if (mnt->tag != TAG_TIMEOUT)
			continue;
		/* If udev was not able to identify the expected device within
		 * ROOTDELAY time, something is wrong */
		if ((! mnt->ready)
		    && (! mnt->nodev)
		    && (! is_remote (mnt))
		    && ((! strncmp (mnt->device, "/dev/", 5))
			|| (! strncmp (mnt->device, "UUID=", 5))
			|| (! strncmp (mnt->device, "LABEL=", 6))))
		{
			nih_message ("%s device not ready in ROOTDELAY sec", MOUNT_NAME (mnt));
			emit_event ("device-not-ready", mnt, NULL);
		}
	}
}

/* 
 * activate_timer:
 *
 * This function is called to start a timer when the first device with
 * "timeout" option is found.
 */
void 
activate_timer (void)
{
	NIH_LIST_FOREACH (mounts, iter) {
		Mount *mnt = (Mount *)iter;

		if (mnt->mounted)
			continue;
		if (mnt->tag != TAG_TIMEOUT)
			continue;
		/* If there's an underlying device that udev is going to deal with,
		 * and it's not a virtual or remote filesystem, we wait for the udev
		 * watcher to mark it ready within ROOTDELAY time.
		 */
		if ((! mnt->ready)
		    && (! mnt->nodev)
		    && (! is_remote (mnt))
		    && ((! strncmp (mnt->device, "/dev/", 5))
			|| (! strncmp (mnt->device, "UUID=", 5))
			|| (! strncmp (mnt->device, "LABEL=", 6))))
		{
			if (!dev_wait_time)
				dev_wait_time = ROOTDELAY;
			nih_debug ("Shall wait for device: %s for %d seconds, starting timer", MOUNT_NAME (mnt), dev_wait_time);
			device_ready_timer = NIH_MUST (nih_timer_add_timeout (NULL, 
						dev_wait_time, is_device_ready, NULL));
			break;

		}

	}

}

void
try_mounts (void)
{
	int all;

	while (newly_mounted) {
		newly_mounted = FALSE;

		all = TRUE;

		NIH_LIST_FOREACH (mounts, iter) {
			Mount *mnt = (Mount *)iter;

			if (mnt->tag == TAG_SKIPPED)
				continue;

			if ((! mnt->mounted) || needs_remount (mnt)) {
				all = FALSE;
				try_mount (mnt, FALSE);
			}
		}

		if (all) {
			/* All mounts have been attempted, so wait for
			 * pending events.
			 */
			int still_pending = 0;

			NIH_LIST_FOREACH (mounts, iter) {
				Mount           *mnt = (Mount *)iter;
				DBusPendingCall *pending_call = mnt->pending_call;

				if (!pending_call)
					continue;

				if (! dbus_pending_call_get_completed (pending_call))
				{
					still_pending++;
					continue;
				}
				dbus_pending_call_block (pending_call);
				dbus_pending_call_unref (pending_call);
				mnt->pending_call = NULL;
			}
			if (still_pending > 0)
				return;

			if (control_server) {
				dbus_server_disconnect (control_server);
				dbus_server_unref (control_server);
			}
			nih_main_loop_exit (EXIT_OK);
		}
	}
}

void
try_mount (Mount *mnt,
	   int    force)
{
	struct stat sb;

	nih_assert (mnt != NULL);

	if (mnt->pending_call)
	        return;

	NIH_LIST_FOREACH (&mnt->deps, dep_iter) {
		NihListEntry *dep_entry = (NihListEntry *)dep_iter;
		Mount *       dep = (Mount *)dep_entry->data;

		if ((! dep->mounted) || needs_remount (dep)) {
			nih_debug ("%s waiting for %s", MOUNT_NAME (mnt),
				   dep->mountpoint);
			return;
		}
	}

	/* If there's an underlying device that udev is going to deal with,
	 * and it's not a virtual or remote filesystem, we wait for the udev
	 * watcher to mark it ready.
	 */
	if ((! mnt->ready)
	    && (! force)
	    && (! mnt->nodev)
	    && (! is_remote (mnt))
	    && ((! strncmp (mnt->device, "/dev/", 5))
		|| (! strncmp (mnt->device, "UUID=", 5))
		|| (! strncmp (mnt->device, "LABEL=", 6))))
	{
		nih_debug ("%s waiting for device", MOUNT_NAME (mnt));

		return;
	}

	/* By this point any parent mounts have appeared, so we can check
	 * whether the mountpoint is a symlink to somewhere else.
	 */
	if (! mnt->checked_link
	    && strcmp (mnt->type, "swap")
	    && strcmp (mnt->mountpoint, "none")
	    && ! lstat (mnt->mountpoint, &sb)
	    && S_ISLNK (sb.st_mode))
	{
		nih_local char *buf = NULL;
		size_t          bufsz;
		ssize_t         linksz;
		Mount          *target = NULL;

		bufsz = 4096;
		buf = NIH_MUST (nih_alloc (NULL, bufsz));

		linksz = readlink (mnt->mountpoint, buf, bufsz);
		if (linksz > 0)
		{
			buf[linksz] = '\0';
			target = find_mount (buf);
		}

		if (target)
		{
			NihListEntry *dep;

			dep = NIH_MUST (nih_list_entry_new (mnt));
			dep->data = target;
			nih_ref (target, dep);

			nih_list_add (&mnt->deps, &dep->entry);
			nih_debug ("%s symlink to %s",
				   MOUNT_NAME (mnt),
				   MOUNT_NAME (target));

			mnt->link_target = target;

			if ((! target->mounted) || needs_remount (target))
			{
				mnt->checked_link = TRUE;
				nih_debug ("%s waiting for %s",
					   MOUNT_NAME (mnt),
					   MOUNT_NAME (target));
				return;
			}
		}
	}
	mnt->checked_link = TRUE;

	if (mnt->link_target)
	{
		nih_debug ("considering %s mounted, as a symlink to %s",
			   MOUNT_NAME (mnt), MOUNT_NAME (mnt->link_target));
		mnt->ready = TRUE;
		mounted (mnt);
		return;
	}

	/* A mount is already in progress for this mount point; do not
	 * send an extra event, which will just confuse things
	 */
	if (mnt->mount_pid > 0)
		return;
	/* Queue a filesystem check if not yet ready, otherwise emit mounting
	 * event (callback will run swapon or mount as appropriate).
	 */
	if ((! mnt->ready)
	    && (! is_remote (mnt))) {
		run_fsck (mnt);
	} else if (! strcmp (mnt->type, "swap")) {
		nih_info ("mounting event sent for swap %s", mnt->device);
		emit_event ("mounting", mnt, mounting_event_handled);
	} else {
		nih_info ("mounting event sent for %s", mnt->mountpoint);
		emit_event ("mounting", mnt, mounting_event_handled);
	}
}


void
mounting_event_handled (void *data,
                        NihDBusMessage *message)
{
	Mount *mnt = ((EventReplyData *)data)->mnt;

	nih_free (data);

	/* We may generate new pending events below; make sure to clear
	 * the current one before we do. */
	mnt->pending_call = NULL;

	if (!strcmp(mnt->type, "swap")) {
		nih_info ("mounting event handled for swap %s", mnt->device);
		run_swapon (mnt);
	} else {
		nih_info ("mounting event handled for %s", mnt->mountpoint);
		run_mount (mnt, FALSE);
	}
}


void
mounted_event_handled (void *data,
                       NihDBusMessage *message)
{
	Mount *mnt = ((EventReplyData *)data)->mnt;

	nih_free (data);

	/* We may generate new pending events below; make sure to clear
	 * the current one before we do. */
	mnt->pending_call = NULL;

	mnt->mounted = TRUE;
	newly_mounted = TRUE;
	nih_main_loop_interrupt ();

	if (!strcmp(mnt->type, "swap")) {
		nih_info ("mounted event handled for swap %s", mnt->device);
	} else {
		nih_info ("mounted event handled for %s", mnt->mountpoint);
	}

	/* Any previous mount options no longer apply
	 * (ie. we're not read-only anymore)
	 */
	if (mnt->mount_opts)
		nih_unref (mnt->mount_opts, mounts);
	mnt->mount_opts = NULL;

	if ((! written_mtab))
		write_mtab ();
	if (written_mtab && mnt->needs_mtab)
		run_mount (mnt, TRUE);

	switch (mnt->tag) {
	case TAG_LOCAL:
		num_local_mounted++;
		break;
	case TAG_TIMEOUT:
		num_local_mounted++;
		num_timeout_mounted++;
		if (num_timeout_mounted == num_timeout) {
			nih_message (_("\n %s finished! "), "local_timeout");
			/* Stop the timeout waiting for device to get ready"
			 */
			if (device_ready_timer) {
				nih_free (device_ready_timer);
				device_ready_timer = NULL;
			}
		}
		break;
	case TAG_REMOTE:
		num_remote_mounted++;
		break;
	case TAG_VIRTUAL:
		num_virtual_mounted++;
		break;
	case TAG_SWAP:
		num_swap_mounted++;
		break;
	case TAG_NOWAIT:
		break;
	case TAG_SKIPPED:
		break;
	case TAG_UNKNOWN:
		break;
	default:
		nih_assert_not_reached ();
	}

	trigger_events ();

	fsck_update ();
}


/**
 * find_on_path:
 *
 * Returns TRUE if the given name is on the executable search path.
 **/
int
find_on_path (const char *name)
{
	nih_local char *path = NULL;
	char *          pathtok;
	const char *    element;

	path = getenv ("PATH");
	if (!path)
		return FALSE;
	pathtok = path = NIH_MUST (nih_strdup (NULL, path));

	do {
		nih_local char *filename = NULL;
		struct stat     st;

		element = strsep (&pathtok, ":");
		filename = NIH_MUST (nih_sprintf (NULL, "%s/%s",
						  element, name));
		if (stat (filename, &st) == 0)
			return TRUE;
	} while (element);

	return FALSE;
}


/**
 * find_fsck:
 *
 * Returns TRUE if the given filesystem type has a fsck.* checker on the
 * executable search path.
 **/
int
find_fsck (const char *type)
{
	if (strncmp (type, "fsck.", 5) == 0)
		return find_on_path (type);
	else {
		nih_local char *fsck_type = NULL;

		fsck_type = NIH_MUST (nih_sprintf (NULL, "fsck.%s", type));
		return find_on_path (fsck_type);
	}
}


pid_t
spawn (Mount *         mnt,
       char * const *  args,
       int             wait,
       void (*handler) (Mount *mnt, pid_t pid, int status))
{
	pid_t    pid;
	int      fds[2];
	char     flag = '!';
	Process *proc;

	nih_assert (mnt != NULL);
	nih_assert (args != NULL);
	nih_assert (args[0] != NULL);

	if (pipe2 (fds, O_CLOEXEC) < 0) {
		nih_fatal ("Unable to create pipe for spawned process: %s",
			   strerror (errno));
		exit (EXIT_ERROR);
	}

	fflush (stdout);
	fflush (stderr);

	pid = fork ();
	if (pid < 0) {
		close (fds[0]);
		close (fds[1]);

		nih_fatal ("%s %s: %s", args[0], MOUNT_NAME (mnt),
			   strerror (errno));
		exit (EXIT_ERROR);
	} else if (! pid) {
		nih_local char *msg = NULL;

		if (setpgid (0, 0) < 0)
			nih_warn (_("setpgid failed: %s"), strerror (errno));

		for (char * const *arg = args; arg && *arg; arg++)
			NIH_MUST (nih_strcat_sprintf (&msg, NULL, msg ? " %s" : "%s", *arg));

		nih_debug ("%s", msg);

		fflush (stdout);
		fflush (stderr);
		execvp (args[0], args);
		nih_fatal ("%s %s [%d]: %s", args[0], MOUNT_NAME (mnt),
			   getpid (), strerror (errno));
		nih_assert (write (fds[1], &flag, 1) == 1);
		exit (0);
	} else {
		int ret;

		close (fds[1]);
		ret = read (fds[0], &flag, 1);
		close (fds[0]);

		if (ret > 0)
			exit (EXIT_ERROR);
	}

	nih_debug ("%s %s [%d]", args[0], MOUNT_NAME (mnt), pid);

	proc = NIH_MUST (nih_new (NULL, Process));

	proc->mnt = mnt;

	proc->args = args;
	nih_ref (proc->args, proc);

	proc->pid = pid;
	proc->handler = handler;

	if (wait) {
		siginfo_t info;

		if (waitid (P_PID, pid, &info, WEXITED) < 0) {
			nih_fatal ("Unable to obtain process exit status: %s",
				   strerror (errno));
			exit (EXIT_ERROR);
		}

		spawn_child_handler (proc, pid, info.si_code == CLD_EXITED ? NIH_CHILD_EXITED : NIH_CHILD_KILLED,
				     info.si_status);
		return 0;
	} else {
		NIH_MUST (nih_child_add_watch (NULL, pid, NIH_CHILD_EXITED|NIH_CHILD_KILLED|NIH_CHILD_DUMPED,
					       (NihChildHandler)spawn_child_handler, proc));
		return pid;
	}
}

void
spawn_child_handler (Process *      proc,
		     pid_t          pid,
		     NihChildEvents event,
		     int            status)
{
	nih_assert (proc != NULL);
	nih_assert (pid == proc->pid);

	if (event != NIH_CHILD_EXITED) {
		const char *sig;

		sig = nih_signal_to_name (status);
		if (sig) {
			nih_fatal ("%s %s [%d] killed by %s signal", proc->args[0],
				   MOUNT_NAME (proc->mnt), pid, sig);
		} else {
			nih_fatal ("%s %s [%d] killed by signal %d", proc->args[0],
				   MOUNT_NAME (proc->mnt), pid, status);
		}

		status <<= 8;
	} else if (status) {
		nih_warn (_("%s %s [%d] terminated with status %d"),
			  proc->args[0],
			  MOUNT_NAME (proc->mnt), pid, status);
	} else {
		nih_info (_("%s %s [%d] exited normally"), proc->args[0],
			  MOUNT_NAME (proc->mnt), pid);
	}

	if (proc->handler)
		proc->handler (proc->mnt, pid, status);

	nih_free (proc);
}


void
run_mount (Mount *mnt,
	   int    fake)
{
	nih_local char **args = NULL;
	size_t           args_len = 0;
	nih_local char * opts = NULL;

	nih_assert (mnt != NULL);

	if (fake) {
		nih_debug ("mtab %s", MOUNT_NAME (mnt));
	} else if (mnt->mount_pid > 0) {
		nih_debug ("%s: already mounting", MOUNT_NAME (mnt));
		return;
	} else if (mnt->error > ERROR_BORED) {
		nih_debug ("%s still has uncleared errors", MOUNT_NAME (mnt));
		return;
	} else if (mnt->mounted) {
		if (needs_remount (mnt)) {
			nih_info (_("remounting %s"), MOUNT_NAME (mnt));
		} else {
			nih_debug ("%s: already mounted", MOUNT_NAME (mnt));
			return;
		}
	} else if (mnt->nodev
		   && (! strcmp (mnt->type, "none"))) {
		nih_debug ("%s: placeholder", MOUNT_NAME (mnt));
		mounted (mnt);
		return;
	} else {
		nih_info (_("mounting %s"), MOUNT_NAME (mnt));
	}

	if (mkdir (mnt->mountpoint, 0755) < 0) {
		/* If this is optional, the mountpoint might not exist
		 * (don't check otherwise, let mount worry about it - since
		 *  some fuse module might make them :p)
		 */
		if ((errno != EEXIST)
		    && has_option (mnt, "optional", FALSE)) {
			nih_debug ("%s: mountpoint doesn't exist; ignoring",
				   MOUNT_NAME (mnt));
			mounted (mnt);
			return;
		}
	}

	opts = cut_options (NULL, mnt, "showthrough", "optional",
			    "bootwait", "nobootwait", "timeout",
			    NULL);
	if (mnt->mounted && (! fake)) {
		char *tmp;

		tmp = NIH_MUST (nih_strdup (NULL, "remount,"));
		NIH_MUST (nih_strcat (&tmp, NULL, opts));

		nih_discard (opts);
		opts = tmp;
	}

	args = NIH_MUST (nih_str_array_new (NULL));
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "mount"));
	if (fake) {
		NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "-f"));
		/* Broken mount helpers that don't support -f, so just bypass
		 * them; no custom mtab for you!
		 */
		if ((! strcmp (mnt->type, "ecryptfs"))
		    || (! strcmp (mnt->type, "aufs")))
			NIH_MUST (nih_str_array_add (&args, NULL, &args_len,
						     "-i"));
	} else if ((! written_mtab)
		   && strcmp (mnt->type, "ntfs")
		   && strcmp (mnt->type, "ntfs-3g")) {
		NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "-n"));
		mnt->needs_mtab = TRUE;
	} else if (mnt->has_showthrough) {
		NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "-n"));
	}
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "-t"));
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, mnt->type));
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "-o"));
	NIH_MUST (nih_str_array_addp (&args, NULL, &args_len, opts));
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, mnt->device));
	if (mnt->has_showthrough && (! fake)) {
		nih_local char *mountpoint = NULL;

		mountpoint = NIH_MUST (nih_sprintf (NULL, "/dev/%s",
						    mnt->mountpoint));

		for (size_t i = 5; i < strlen (mountpoint); i++)
			if (mountpoint[i] == '/')
				mountpoint[i] = '.';

		nih_debug ("diverting mount to %s", mountpoint);
		if ((mkdir (mountpoint, 0755) < 0)
		    && (errno != EEXIST)) {
			nih_fatal ("mkdir %s: %s", mountpoint, strerror (errno));
			exit (EXIT_ERROR);
		} else
			NIH_MUST (nih_str_array_add (&args, NULL, &args_len, mountpoint));
	} else {
		NIH_MUST (nih_str_array_add (&args, NULL, &args_len, mnt->mountpoint));
	}

	if (fake) {
 		spawn (mnt, args, TRUE, NULL);
	} else if (! is_remote (mnt)) {
 		spawn (mnt, args, TRUE, run_mount_finished);
	} else {
		mnt->error = ERROR_NONE;
		mnt->mount_pid = spawn (mnt, args, FALSE, run_mount_finished);
	}
}

void
run_mount_finished (Mount *mnt,
		    pid_t  pid,
		    int    status)
{
	nih_assert (mnt != NULL);
	nih_assert ((mnt->mount_pid == pid)
		    || (mnt->mount_pid == -1));

	mnt->error = ERROR_NONE;
	mnt->mount_pid = -1;

	if (status) {
		if (! is_remote (mnt)) {
			nih_error (_("Filesystem could not be mounted: %s"),
				   MOUNT_NAME (mnt));

			mnt->error = ERROR_MOUNT_FAILED;
			plymouth_update (FALSE);
		} else {
			nih_info (_("Filesystem could not be mounted: %s"),
				   MOUNT_NAME (mnt));
		}

		return;
	}

	if (mnt->has_showthrough)
		mount_showthrough (mnt);

	/* Parse mountinfo to see what mount did; in particular to update
	 * the type if multiple types are listed in fstab.
	 */
	if (! mnt->mounted) {
		parse_mountinfo ();
		/* If the canonical path of the mount point doesn't match
		 * what's in the fstab, reparsing /proc/mounts won't change
		 * this mount but instead create a new record.  So handle
		 * this case directly here.
		 */
		if (! mnt->mounted && ! mnt->pending_call)
			mounted (mnt);
	} else {
		mounted (mnt);
	}
}


void
run_swapon (Mount *mnt)
{
	nih_local char **args = NULL;
	size_t           args_len = 0;
	nih_local char * pri = NULL;

	nih_assert (mnt != NULL);

	if (mnt->mounted) {
		nih_debug ("%s: already activated", MOUNT_NAME (mnt));
		return;
	} else if (mnt->mount_pid > 0) {
		nih_debug ("%s: already activating", MOUNT_NAME (mnt));
		return;
	} else if (mnt->error > ERROR_BORED) {
		nih_debug ("%s still has uncleared errors", MOUNT_NAME (mnt));
		return;
	}

	nih_info (_("activating %s"), MOUNT_NAME (mnt));

	args = NIH_MUST (nih_str_array_new (NULL));
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "swapon"));

	if (((pri = get_option (NULL, mnt, "pri", FALSE)) != NULL)
	    && *pri) {
		NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "-p"));
		NIH_MUST (nih_str_array_add (&args, NULL, &args_len, pri));
	}
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, mnt->device));

	spawn (mnt, args, TRUE, run_swapon_finished);
}

void
run_swapon_finished (Mount *mnt,
		     pid_t  pid,
		     int    status)
{
	nih_assert (mnt != NULL);
	nih_assert ((mnt->mount_pid == pid)
		    || (mnt->mount_pid == -1));

	mnt->mount_pid = -1;

	/* Swapon doesn't return any useful status codes, so we just
	 * carry on regardless if it failed.
	 */
	if (status)
		nih_warn (_("Problem activating swap: %s"), MOUNT_NAME (mnt));

	mounted (mnt);
}


static const char *disregard_missing_fsck[] = {
	"ext2",
	"ext3",
	"ext4",
	"ext4dev",
	"jfs",
	"reiserfs",
	"xfs",
	NULL
};

/**
 * missing_fsck:
 *
 * Returns TRUE if fsck on this mount should be skipped due to a missing
 * fsck.* checker.
 */
int
missing_fsck (Mount *mnt)
{
	const char * const *disregard;

	for (disregard = disregard_missing_fsck; *disregard; ++disregard) {
		if (strcmp (mnt->type, *disregard) == 0)
			/* For this filesystem type, it isn't OK to just
			 * ignore a missing checker, so pretend that it's
			 * present for the time being in order that
			 * run_fsck_finished will display an error message.
			 */
			return FALSE;
	}

	if (find_fsck (mnt->type))
		return FALSE;

	return TRUE;
}


void
run_fsck (Mount *mnt)
{
	nih_local char **args = NULL;
	size_t           args_len = 0;
	int              fds[2];
	int              flags;

	nih_assert (mnt != NULL);

	if (mnt->ready) {
		nih_debug ("%s: already ready", MOUNT_NAME (mnt));
		try_mount (mnt, FALSE);
		return;
	} else if (! mnt->check
		   && (! force_fsck || strcmp (mnt->mountpoint, "/"))) {
		nih_debug ("%s: no check required", MOUNT_NAME (mnt));
		mnt->ready = TRUE;
		try_mount (mnt, FALSE);
		return;
	} else if (missing_fsck (mnt)) {
		nih_debug ("%s: no appropriate fsck.* present",
			   MOUNT_NAME (mnt));
		mnt->ready = TRUE;
		try_mount (mnt, FALSE);
		return;
	} else if (mnt->nodev
		   || (! strcmp (mnt->type, "none"))) {
		nih_debug ("%s: no device to check", MOUNT_NAME (mnt));
		mnt->ready = TRUE;
		try_mount (mnt, FALSE);
		return;
	} else if (mnt->mounted && (! has_option (mnt, "ro", TRUE))) {
		nih_debug ("%s: mounted filesystem", MOUNT_NAME (mnt));
		mnt->ready = TRUE;
		try_mount (mnt, FALSE);
		return;
	} else if (mnt->fsck_pid > 0) {
		nih_debug ("%s: already checking", MOUNT_NAME (mnt));
		return;
	} else if (mnt->error > ERROR_BORED) {
		nih_debug ("%s still has uncleared errors", MOUNT_NAME (mnt));
		return;
	}

	nih_info (_("checking %s"), MOUNT_NAME (mnt));

	/* Create a pipe to receive progress indication */
	if (pipe2 (fds, O_CLOEXEC) < 0) {
		nih_fatal ("Unable to create pipe for spawned process: %s",
			   strerror (errno));
		exit (EXIT_ERROR);
	}

	flags = fcntl (fds[1], F_GETFD);
	if ((flags < 0)
	    || (fcntl (fds[1], F_SETFD, flags &~ FD_CLOEXEC) < 0)
	    || (! NIH_SHOULD (nih_io_reopen (NULL, fds[0], NIH_IO_STREAM,
					     (NihIoReader)fsck_reader,
					     NULL, NULL, mnt)))) {
		close (fds[0]);
		close (fds[1]);
		fds[0] = fds[1] = -1;
	}

	args = NIH_MUST (nih_str_array_new (NULL));
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "fsck"));
	if (fsck_fix || mnt->fsck_fix) {
		NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "-y"));
	} else {
		NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "-a"));
	}
	if (fds[1] >= 0) {
		nih_local char *arg = NULL;

		arg = NIH_MUST (nih_sprintf (NULL, "-C%d", fds[1]));
		NIH_MUST (nih_str_array_addp (&args, NULL, &args_len, arg));
	}
	if (force_fsck)
		NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "-f"));
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "-t"));
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, mnt->type));
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, mnt->device));

	mnt->error = ERROR_NONE;
	mnt->fsck_pid = spawn (mnt, args, FALSE, run_fsck_finished);
	mnt->fsck_progress = -1;

	fsck_update ();

	/* Close writing end, reading end is left open inside the NihIo
	 * structure watching it until the remote end is closed by fsck
	 * finishing.
	 */
	if (fds[1] >= 0)
		close (fds[1]);
}

void
run_fsck_finished (Mount *mnt,
		   pid_t  pid,
		   int    status)
{
	nih_assert (mnt != NULL);
	nih_assert (mnt->fsck_pid == pid);

	/* Finish off the progress bar */
	plymouth_progress (mnt, 100);

	/* The check is done */
	mnt->error = ERROR_NONE;
	mnt->fsck_pid = -1;
	mnt->fsck_progress = -1;

 	fsck_update ();

	/* Handle errors */
	if (status & 2) {
		nih_error (_("System must be rebooted: %s"), MOUNT_NAME (mnt));
		exit (EXIT_REBOOT);

 	} else if ((status & (4 | 8 | 16 | 128)) || (status > 255)) {
		if ((status & 4) && (! fsck_fix) && (! mnt->fsck_fix)) {
			nih_error (_("Filesystem has errors: %s"),
				   MOUNT_NAME (mnt));

			mnt->error = ERROR_FSCK_FAILED;
		} else {
			nih_error (_("Unrecoverable fsck error: %s"),
				   MOUNT_NAME (mnt));

			mnt->error = ERROR_FSCK_FAILED_HARD;
		}

		plymouth_update (FALSE);
		return;

	} else if ((status & 32) || (status == SIGTERM)) {
		nih_info (_("Filesytem check cancelled: %s"), MOUNT_NAME (mnt));

		/* Fall through */

	} else if (status & 1) {
		nih_info (_("Filesystem errors corrected: %s"), MOUNT_NAME (mnt));

		/* Fall through */

	}

	mnt->ready = TRUE;
	try_mount (mnt, FALSE);
}


void
write_mtab (void)
{
	int mtab;

	if (((mtab = open (_PATH_MOUNTED, O_CREAT | O_TRUNC | O_NOFOLLOW | O_WRONLY, 0644)) < 0)
	    || (close (mtab) < 0))
		return;

	unlink (_PATH_MOUNTED "~");

	NIH_LIST_FOREACH (mounts, iter) {
		Mount *mnt = (Mount *)iter;

		if (! mnt->mounted)
			continue;
		if (mnt->nodev
		    && (! strcmp (mnt->type, "none")))
			continue;
		if (! strcmp (mnt->type, "swap"))
			continue;
		if (mnt->link_target)
			continue;

		run_mount (mnt, TRUE);
		mnt->needs_mtab = FALSE;
	}

	written_mtab = TRUE;
}


void
mount_showthrough (Mount *root)
{
	nih_local char * mountpoint = NULL;
	nih_local char **args = NULL;
	size_t           args_len = 0;

	nih_assert (root != NULL);
	nih_assert (root->has_showthrough);

	/* This is the mountpoint we actually used */
	mountpoint = NIH_MUST (nih_sprintf (NULL, "/dev/%s", root->mountpoint));
	for (size_t i = 5; i < strlen (mountpoint); i++)
		if (mountpoint[i] == '/')
			mountpoint[i] = '.';

	NIH_LIST_FOREACH (mounts, iter) {
		Mount *          mnt = (Mount *)iter;
		nih_local char * submount = NULL;
		nih_local char **args = NULL;
		size_t           args_len = 0;

		if (mnt->showthrough != root)
			continue;
		if (! mnt->mounted)
			continue;

		submount = NIH_MUST (nih_sprintf (NULL, "%s%s", mountpoint,
						  mnt->mountpoint + strlen (root->mountpoint)));

		args = NIH_MUST (nih_str_array_new (NULL));
		NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "mount"));
		NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "-n"));
		NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "--bind"));
		NIH_MUST (nih_str_array_add (&args, NULL, &args_len, mnt->mountpoint));
		NIH_MUST (nih_str_array_add (&args, NULL, &args_len, submount));

		nih_debug ("binding %s to %s", mnt->mountpoint, submount);

		if (spawn (mnt, args, TRUE, NULL) < 0)
			return;
	}

	/* Now move the root mountpoint into the right place, along with
	 * all the bound mounts under it.
	 */
	args = NIH_MUST (nih_str_array_new (NULL));
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "mount"));
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "-n"));
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, "--move"));
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, mountpoint));
	NIH_MUST (nih_str_array_add (&args, NULL, &args_len, root->mountpoint));

	nih_debug ("moving %s", root->mountpoint);

	if (spawn (root, args, TRUE, NULL) < 0)
		return;

	if ((rmdir (mountpoint) < 0)
	    && (errno != EEXIST))
		nih_warn (_("rmdir %s failed: %s"),
			  mountpoint, strerror (errno));

	if (written_mtab)
		run_mount (root, TRUE);
}


void
upstart_disconnected (DBusConnection *connection)
{
	nih_fatal (_("Disconnected from Upstart"));
	exit (EXIT_ERROR);
}

void
emit_event (const char *name,
	    Mount *     mnt,
	    void (*cb) (void *, NihDBusMessage *message))
{
	DBusPendingCall *pending_call;
	nih_local char **env = NULL;
	size_t           env_len = 0;
	EventReplyData  *reply_data = NULL;

	nih_assert (name != NULL);

	if (mnt && cb) {
		reply_data = NIH_MUST (nih_alloc (NULL, sizeof (EventReplyData)));
		reply_data->mnt = mnt;
		reply_data->handler = cb;
	}

	if (no_events) {
		if (cb)
			cb (reply_data, NULL);
		return;
	}

	/* Flush the Plymouth connection to ensure all updates are sent,
	 * since the event may kill plymouth.
	 */
	if (ply_boot_client)
		ply_boot_client_flush (ply_boot_client);

	env = NIH_MUST (nih_str_array_new (NULL));

	if (mnt) {
		char *var;

		var = NIH_MUST (nih_sprintf (NULL, "DEVICE=%s",
					     mnt->device));
		NIH_MUST (nih_str_array_addp (&env, NULL, &env_len, var));
		nih_discard (var);

		var = NIH_MUST (nih_sprintf (NULL, "MOUNTPOINT=%s",
					     mnt->mountpoint));
		NIH_MUST (nih_str_array_addp (&env, NULL, &env_len, var));
		nih_discard (var);

		var = NIH_MUST (nih_sprintf (NULL, "TYPE=%s",
					     mnt->type));
		NIH_MUST (nih_str_array_addp (&env, NULL, &env_len, var));
		nih_discard (var);

		var = NIH_MUST (nih_sprintf (NULL, "OPTIONS=%s",
					     mnt->opts));
		NIH_MUST (nih_str_array_addp (&env, NULL, &env_len, var));
		nih_discard (var);
	}

	pending_call = NIH_SHOULD (upstart_emit_event (upstart,
						       name, env, reply_data ? TRUE : FALSE,
						       reply_data ? cb : NULL,
						       emit_event_error, reply_data,
						       NIH_DBUS_TIMEOUT_NEVER));

	if (pending_call) {
	
		if (reply_data) {

			/* If previous event is still pending, wait for it. */
			if (mnt->pending_call) {
				DBusPendingCall *pending = mnt->pending_call;
				dbus_pending_call_block (pending);
				dbus_pending_call_unref (pending);
			}

			mnt->pending_call = pending_call;

		} else {

                        /* Immediate return from upstart, wait for it. */
			dbus_pending_call_block (pending_call);
			dbus_pending_call_unref (pending_call);
		}
		
	} else {
	
		NihError *err;

		err = nih_error_get ();
		nih_warn ("%s", err->message);
		nih_free (err);

		nih_free (reply_data);
	}
}

void
emit_event_error (void *          data,
		  NihDBusMessage *message)
{
	EventReplyData *reply_data = (EventReplyData *)data;
	NihError *err;

	err = nih_error_get ();
	nih_warn ("%s", err->message);
	nih_free (err);

	/* Even if the event returned an error, we still want to do the mount */
	if (reply_data)
		reply_data->handler (data, message);
}


static void
try_udev_device (struct udev_device *udev_device)
{
	const char *            action;
	const char *            subsystem;
	const char *            kernel;
	const char *            devname;
	const char *            usage;
	const char *            type;
	const char *            uuid;
	const char *            label;
	struct udev_list_entry *devlinks;

	action    = udev_device_get_action (udev_device);
	subsystem = udev_device_get_subsystem (udev_device);
	kernel    = udev_device_get_sysname (udev_device);
	devname   = udev_device_get_devnode (udev_device);
	devlinks  = udev_device_get_devlinks_list_entry (udev_device);
	usage     = udev_device_get_property_value (udev_device, "ID_FS_USAGE");
	type      = udev_device_get_property_value (udev_device, "ID_FS_TYPE");
	uuid      = udev_device_get_property_value (udev_device, "ID_FS_UUID");
	label     = udev_device_get_property_value (udev_device, "ID_FS_LABEL");

	if ((! subsystem)
	    || strcmp (subsystem, "block"))
		return;

	if (action
	    && strcmp (action, "add")
	    && strcmp (action, "change"))
		return;

	/* devmapper, md, loop and ram devices must be "ready" before
	 * we'll try them - as must any device we found without udev.
	 */
	if ((! action)
	    || (! strncmp (kernel, "dm-", 3))
	    || (! strncmp (kernel, "md", 2))
	    || (! strncmp (kernel, "loop", 4))
	    || (! strncmp (kernel, "ram", 3))) {
		if ((! usage) && (! type) && (! uuid) && (! label)) {
			if (action)
				nih_debug ("ignored %s (not yet ready?)", devname);
			return;
		}
	}

	nih_debug ("%s %s %s %s", subsystem, devname, uuid, label);

	NIH_LIST_FOREACH (mounts, iter) {
		Mount *mnt = (Mount *)iter;

		if (mnt->nodev)
			continue;

		if ((! strncmp (mnt->device, "UUID=", 5))
		    && uuid
		    && (! strcmp (mnt->device + 5, uuid))) {
			struct udev_list_entry *devlink;

			nih_debug ("%s by uuid", MOUNT_NAME (mnt));

			for (devlink = devlinks; devlink;
			     devlink = udev_list_entry_get_next (devlink)) {
				const char *name = udev_list_entry_get_name (devlink);

				if (! strncmp (name, "/dev/disk/by-uuid/", 18)) {
					update_mount (mnt, name, -1, NULL, NULL);
					break;
				}
			}

			if (! devlink)
				update_mount (mnt, devname, -1, NULL, NULL);
		} else if ((! strncmp (mnt->device, "LABEL=", 6))
			   && label
			   && (! strcmp (mnt->device + 6, label))) {
			struct udev_list_entry *devlink;

			nih_debug ("%s by label", MOUNT_NAME (mnt));

			for (devlink = devlinks; devlink;
			     devlink = udev_list_entry_get_next (devlink)) {
				const char *name = udev_list_entry_get_name (devlink);

				if (! strncmp (name, "/dev/disk/by-label/", 18)) {
					update_mount (mnt, name, -1, NULL, NULL);
					break;
				}
			}

			if (! devlink)
				update_mount (mnt, devname, -1, NULL, NULL);
		} else if (! strcmp (mnt->device, devname)) {
			nih_debug ("%s by name", MOUNT_NAME (mnt));
		} else {
			struct udev_list_entry *devlink;

			for (devlink = devlinks; devlink;
			     devlink = udev_list_entry_get_next (devlink)) {
				const char *name = udev_list_entry_get_name (devlink);

				if (! strcmp (mnt->device, name)) {
					nih_debug ("%s by link %s",
						   MOUNT_NAME (mnt), name);
					break;
				}
			}

			if (! devlink)
				continue;
		}

		if (mnt->udev_device)
			udev_device_unref (mnt->udev_device);

		mnt->udev_device = udev_device;
		udev_device_ref (mnt->udev_device);

		mnt->physical_dev_ids_needed = TRUE;

		run_fsck (mnt);
	}
}

void
udev_monitor_watcher (struct udev_monitor *udev_monitor,
		      NihIoWatch *         watch,
		      NihIoEvents          events)
{
	struct udev_device *udev_device;

	nih_assert (udev_monitor != NULL);

	udev_device = udev_monitor_receive_device (udev_monitor);
	if (! udev_device)
		return;

	try_udev_device (udev_device);

	udev_device_unref (udev_device);
}

void
udev_catchup (void)
{
	struct udev_enumerate * udev_enumerate;
	struct udev_list_entry *device_path;

	nih_assert (udev != NULL);

	udev_enumerate = udev_enumerate_new (udev);
	nih_assert (udev_enumerate_add_match_subsystem (udev_enumerate, "block") == 0);

	nih_debug ("catching up");

	udev_enumerate_scan_devices (udev_enumerate);

	for (device_path = udev_enumerate_get_list_entry (udev_enumerate);
	     device_path != NULL;
	     device_path = udev_list_entry_get_next (device_path)) {
		const char *        path = udev_list_entry_get_name (device_path);
		struct udev_device *udev_device;

		udev_device = udev_device_new_from_syspath (udev, path);
		if (udev_device)
			try_udev_device (udev_device);

		udev_device_unref (udev_device);
	}
}


static void
destroy_device (NihListEntry *entry)
{
	struct udev_device *dev;

	nih_assert (entry != NULL);

	dev = (struct udev_device *)entry->data;
	nih_assert (dev != NULL);

	udev_device_unref (dev);
	nih_list_destroy (&entry->entry);
}

static void
add_device (NihList *           devices,
	    struct udev_device *srcdev,
	    struct udev_device *newdev,
	    size_t *            nadded)
{
	NihListEntry *entry;

	nih_assert (devices != NULL);
	nih_assert (newdev != NULL);

	udev_device_ref (newdev);

	entry = NIH_MUST (nih_list_entry_new (devices));
	entry->data = newdev;
	nih_alloc_set_destructor (entry, destroy_device);
	nih_list_add (devices, &entry->entry);

	if (nadded)
		(*nadded)++;

	if (srcdev)
		nih_debug ("traverse: %s -> %s",
			   udev_device_get_sysname (srcdev),
			   udev_device_get_sysname (newdev));
}

static void
update_physical_dev_ids (Mount *mnt)
{
	nih_local NihList *devices = NULL;

	nih_assert (mnt != NULL);

	if (! mnt->physical_dev_ids_needed)
		return;

	mnt->physical_dev_ids_needed = FALSE;

	if (mnt->physical_dev_ids) {
		nih_free (mnt->physical_dev_ids);
		nih_debug ("recomputing physical_dev_ids for %s",
			   MOUNT_NAME (mnt));
	}

	mnt->physical_dev_ids = NIH_MUST (nih_hash_string_new (mnt, 10));

	devices = NIH_MUST (nih_list_new (NULL));

	if (mnt->udev_device) {
		add_device (devices, NULL, mnt->udev_device, NULL);
	} else {
		struct stat         sb;
		struct udev_device *dev;

		/* Is it a loop file? */

		if ((stat (mnt->device, &sb) == 0)
		    && S_ISREG (sb.st_mode)
		    && (dev = udev_device_new_from_devnum (udev, 'b',
							   sb.st_dev))) {
			add_device (devices, NULL, dev, NULL);
			udev_device_unref (dev);
		} else {
			nih_debug ("%s: couldn't resolve physical devices",
				   MOUNT_NAME (mnt));
		}
	}

	while (! NIH_LIST_EMPTY (devices)) {
		NihListEntry *      entry;

		struct udev_device *dev;
		struct udev_device *newdev;

		size_t              nadded;

		const char *        syspath;
		nih_local char *    slavespath = NULL;

		const char *        dev_id;

		DIR *               dir;
		struct dirent *     ent;

		entry = (NihListEntry *)devices->next;
		dev = (struct udev_device *)entry->data;

		/* Does this device have a parent? */

		newdev = udev_device_get_parent_with_subsystem_devtype (
			dev, "block", "disk");
		if (newdev) {
			add_device (devices, dev, newdev, NULL);
			goto finish;
		}

		/* Does this device have slaves? */

		nadded = 0;

		nih_assert (syspath = udev_device_get_syspath (dev));
		slavespath = NIH_MUST (nih_sprintf (NULL, "%s/slaves",
						    syspath));

		if ((dir = opendir (slavespath))) {
			while ((ent = readdir (dir))) {
				nih_local char *slavepath = NULL;

				if ((! strcmp (ent->d_name, "."))
				    || (! strcmp (ent->d_name, "..")))
					continue;

				slavepath = NIH_MUST (nih_sprintf (NULL,
					"%s/%s", slavespath, ent->d_name));

				newdev = udev_device_new_from_syspath (
					udev, slavepath);
				if (! newdev) {
					nih_warn ("%s: %s", slavepath,
						  "udev_device_new_from_syspath"
						  " failed");
					continue;
				}

				add_device (devices, dev, newdev, &nadded);
				udev_device_unref (newdev);
			}

			closedir (dir);
		} else if (errno != ENOENT) {
			nih_warn ("%s: opendir: %s", slavespath,
				  strerror (errno));
		}

		if (nadded > 0)
			goto finish;

		/* This device has no parents or slaves; we’ve reached the
		 * physical device.
		 */

		dev_id = udev_device_get_sysattr_value (dev, "dev");
		if (dev_id) {
			nih_local NihListEntry *entry = NULL;

			entry = NIH_MUST (nih_list_entry_new (mnt->physical_dev_ids));
			entry->str = NIH_MUST (nih_strdup (entry, dev_id));

			if (nih_hash_add_unique (mnt->physical_dev_ids,
						 &entry->entry)) {
				nih_debug ("results: %s -> %s",
					   MOUNT_NAME (mnt), dev_id);
			}
		} else {
			nih_warn ("%s: failed to get sysattr 'dev'", syspath);
		}

finish:
		nih_free (entry);
	}
}

void
fsck_update (void)
{
	nih_local NihHash *locks = NULL;

	int                ioprio_normal,
			   ioprio_low;

	int                fsck_running = FALSE;

	ioprio_normal = IOPRIO_PRIO_VALUE (IOPRIO_CLASS_BE, IOPRIO_NORM);
	ioprio_low = IOPRIO_PRIO_VALUE (IOPRIO_CLASS_IDLE, 7);

	locks = NIH_MUST (nih_hash_string_new (NULL, 10));

	nih_debug ("updating check priorities");

	NIH_LIST_FOREACH (mounts, iter) {
		Mount *mnt = (Mount *)iter;
		int    low_prio = FALSE;

		if (mnt->fsck_pid <= 0)
			continue;

		fsck_running = TRUE;

		update_physical_dev_ids (mnt);

		/* Set low_prio if something else has already locked one of the
		 * dev_ids.
		 */
		NIH_HASH_FOREACH (mnt->physical_dev_ids, diter) {
			char *dev_id = ((NihListEntry *)diter)->str;

			if (nih_hash_lookup (locks, dev_id)) {
				low_prio = TRUE;
				break;
			}
		}

		if (! low_prio) {
			/* Lock the dev_ids. */
			NIH_HASH_FOREACH (mnt->physical_dev_ids, diter) {
				char *        dev_id;
				NihListEntry *entry;

				dev_id = ((NihListEntry *)diter)->str;

				entry = NIH_MUST (nih_list_entry_new (locks));
				entry->str = dev_id;
				nih_ref (entry->str, entry);

				nih_hash_add (locks, &entry->entry);
			}
		}

		nih_debug ("%s: priority %s",
			   MOUNT_NAME (mnt), low_prio ? "low" : "normal");

		if (ioprio_set (IOPRIO_WHO_PGRP, mnt->fsck_pid,
				low_prio ? ioprio_low : ioprio_normal) < 0)
			nih_warn (_("ioprio_set %d failed: %s"),
				  mnt->fsck_pid, strerror (errno));
	}

	/* Reset timeout each pass through, since activity has taken place;
	 * don't restore the timeout while fsck is running.
	 */
	if (boredom_timer) {
		nih_free (boredom_timer);
		boredom_timer = NULL;
	}

	if (! fsck_running) {
		boredom_timer = NIH_MUST (nih_timer_add_timeout (
						  NULL, BOREDOM_TIMEOUT,
						  boredom_timeout, NULL));

		/* Clear messages from Plymouth */
		fsck_in_progress = FALSE;
		plymouth_update (FALSE);
	}
}


void
fsck_reader (Mount *     mnt,
	     NihIo *     io,
	     const char *buf,
	     size_t      len)
{
	nih_assert (mnt != NULL);
	nih_assert (io != NULL);
	nih_assert (buf != NULL);

	for (;;) {
		nih_local char *line = NULL;
		int             pass;
		int             cur;
		int             max;
		int             progress;

		line = nih_io_get (NULL, io, "\n");
		if ((! line) || (! *line))
			break;

		if (sscanf (line, "%d %d %d", &pass, &cur, &max) < 3)
			continue;

		switch (pass) {
		case 1:
			progress = (cur * 70) / max;
			break;
		case 2:
			progress = 70 + (cur * 20) / max;
			break;
		case 3:
			progress = 90 + (cur * 2) / max;
			break;
		case 4:
			progress = 92 + (cur * 3) / max;
			break;
		case 5:
			progress = 95 + (cur * 5) / max;
			break;
		default:
			nih_assert_not_reached ();
		}

		if (mnt->fsck_progress != progress) {
			plymouth_progress (mnt, progress);
			mnt->fsck_progress = progress;
		}
	}
}


void
boredom_timeout (void *    data,
		 NihTimer *timer)
{
	boredom_timer = NULL;

	NIH_LIST_FOREACH (mounts, iter) {
		Mount *           mnt = (Mount *)iter;
		nih_local char *  message = NULL;
		nih_local char *  answer = NULL;

		if (mnt->mounted && (! needs_remount (mnt)))
			continue;
		if (mnt->fsck_pid > 0)
			continue;
		if (mnt->mount_pid > 0)
			continue;
		if (mnt->pending_call != NULL)
			continue;
		if (mnt->tag == TAG_NOWAIT)
			continue;
		if (mnt->tag == TAG_SKIPPED)
			continue;
		if (mnt->error != ERROR_NONE)
			continue;

		mnt->error = ERROR_BORED;
	}

	plymouth_update (FALSE);
}


int
plymouth_connect (void)
{
	/* If we were already connected, just re-use that connection */
	if (ply_boot_client)
		return TRUE;

	ply_boot_client = ply_boot_client_new ();

	/* Connect to the Plymouth daemon */
	if (! ply_boot_client_connect (ply_boot_client,
				       plymouth_disconnected, NULL)) {
		int saved_errno = errno;
		nih_debug ("Failed to connect to Plymouth: %s",
			   strerror (errno));

		ply_boot_client_free (ply_boot_client);
		ply_boot_client = NULL;

		errno = saved_errno;
		return FALSE;
	}

	/* Attach to the event loop */
	ply_boot_client_attach_to_event_loop (ply_boot_client, ply_event_loop);

	nih_info (_("Connected to Plymouth"));

	return TRUE;
}

void
plymouth_disconnected (void *             user_data,
		       ply_boot_client_t *client)
{
	nih_warn (_("Disconnected from Plymouth"));

	/* Disconnect the client from the daemon, closing the socket */
	ply_boot_client_disconnect (ply_boot_client);

	ply_boot_client_free (ply_boot_client);
	ply_boot_client = NULL;
}


void
plymouth_progress (Mount *mnt,
		   int    progress)
{
	nih_local char *update = NULL;

	nih_assert (mnt != NULL);

	/* No Plymouth => no progress information */
	if (! plymouth_connect ())
		return;

	/* Clamp the progress and sent to plymouth */
	progress = nih_max (nih_min (progress, 100), 0);

	/* When not displaying any other errors, display the fsck in progress
	 * (which silences other errors until done)
	 */
	if (progress < 100)
		fsck_in_progress = TRUE;
	plymouth_update (FALSE);

	/* Don't display progress information over top of other errors */
	if (plymouth_error != ERROR_FSCK_IN_PROGRESS)
		return;

	/* The localised string for plymouth
	 * For example: "fsck:sda1:50:Checking disk 1 of 3 (50 % complete)"
	 */
	update = NIH_MUST (nih_sprintf (NULL, "fsck:%s:%d:%s",
					MOUNT_NAME (mnt), progress,
					_("Checking disk %1$d of %2$d (%3$d%% complete)")));

	ply_boot_client_update_daemon (ply_boot_client, update,
				       plymouth_response,
				       plymouth_failed,
				       NULL);
}

void
plymouth_update (int only_clear)
{
	nih_local char *message = NULL;
	nih_local char *keys_message = NULL;

	/* If we're not connected to Plymouth, and can't connect, deal with
	 * all errors (except boredom) by ignoring the filesystem.
	 */
	if (! plymouth_connect ()) {
		NIH_LIST_FOREACH (mounts, iter) {
			Mount *mnt = (Mount *)iter;

			if (mnt->error <= ERROR_BORED)
				continue;

			nih_error (_("Skipping mounting %s since Plymouth is not available"),
				   MOUNT_NAME (mnt));

			mnt->error = ERROR_NONE;

			skip_mount (mnt);
		}

		return;
	}

	/* If we're already displaying messages, don't change them unless
	 * the message is no longer valid for that mount point; otherwise
	 * clear the message.
	 */
	if (plymouth_error != ERROR_NONE) {
		if (plymouth_mnt && (plymouth_mnt->error == plymouth_error))
			return;
		if ((plymouth_error == ERROR_FSCK_IN_PROGRESS)
		    && fsck_in_progress)
			return;

		ply_boot_client_tell_daemon_to_display_message (ply_boot_client,
								"",
								plymouth_response,
								plymouth_failed,
								NULL);
		ply_boot_client_tell_daemon_to_display_message (ply_boot_client,
								"keys:",
								plymouth_response,
								plymouth_failed,
								NULL);
		ply_boot_client_ask_daemon_to_ignore_keystroke (ply_boot_client,
								plymouth_keys,
								(ply_boot_client_answer_handler_t)plymouth_response,
								plymouth_failed,
								NULL);

		plymouth_error = ERROR_NONE;
		plymouth_mnt = NULL;
		plymouth_keys = NULL;
	}

	if (only_clear)
		return;


	/* Look for the best message to display */
	if (fsck_in_progress)
		plymouth_error = ERROR_FSCK_IN_PROGRESS;

	NIH_LIST_FOREACH (mounts, iter) {
		Mount *mnt = (Mount *)iter;

		if (mnt->error > plymouth_error) {
			plymouth_mnt = mnt;
			plymouth_error = mnt->error;
		}
	}

	if (plymouth_error == ERROR_NONE)
		return;

	/* Display that message */
	switch (plymouth_error) {
	case ERROR_BORED:
		message = NIH_MUST (nih_sprintf (NULL,_("The disk drive for %s is not ready yet or not present."),
						 MOUNT_NAME (plymouth_mnt)));
		keys_message = NIH_MUST (nih_sprintf (NULL, "keys:%s",
						      _("Continue to wait, or Press S to skip mounting or M for manual recovery")));
		plymouth_keys = "SsMm";
		break;
	case ERROR_FSCK_FAILED:
		message = NIH_MUST (nih_sprintf (NULL,_("Errors were found while checking the disk drive for %s."),
						 MOUNT_NAME (plymouth_mnt)));
		keys_message = NIH_MUST (nih_sprintf (NULL, "keys:%s",
						      _("Press F to attempt to fix the errors, I to ignore, S to skip mounting, or M for manual recovery")));
		plymouth_keys = "FfIiSsMm";
		break;
	case ERROR_FSCK_FAILED_HARD:
		message = NIH_MUST (nih_sprintf (NULL,_("Serious errors were found while checking the disk drive for %s."),
						 MOUNT_NAME (plymouth_mnt)));
		keys_message = NIH_MUST (nih_sprintf (NULL, "keys:%s",
						      _("Press I to ignore, S to skip mounting, or M for manual recovery")));
		plymouth_keys = "IiSsMm";
		break;
	case ERROR_MOUNT_FAILED:
		message = NIH_MUST (nih_sprintf (NULL, _("An error occurred while mounting %s."),
						 MOUNT_NAME (plymouth_mnt)));
		keys_message = NIH_MUST (nih_sprintf (NULL, "keys:%s",
						      _("Press S to skip mounting or M for manual recovery")));
		plymouth_keys = "SsMm";
		break;
	case ERROR_FSCK_IN_PROGRESS:
		message = NIH_MUST (nih_strdup (NULL, _("Checking disk drives for errors. This may take several minutes.")));
		keys_message = NIH_MUST (nih_sprintf (NULL, "keys:%s",
						      _("Press C to cancel all checks in progress")));
		plymouth_keys = "Cc";
		break;
	default:
		nih_assert_not_reached ();
	}

	/* Prepend the prefix so that plymouth knows that the message
	 * may require an action.
	 */
	ply_boot_client_tell_daemon_to_display_message (ply_boot_client,
							message,
							plymouth_response,
							plymouth_failed,
							NULL);
	ply_boot_client_tell_daemon_to_display_message (ply_boot_client,
							keys_message,
							plymouth_response,
							plymouth_failed,
							NULL);
	ply_boot_client_ask_daemon_to_watch_for_keystroke (ply_boot_client,
							   plymouth_keys,
							   plymouth_answer,
							   plymouth_failed,
							   NULL);
}

void
plymouth_response (void *             user_data,
		   ply_boot_client_t *client)
{
	/* No response to successful command */
}

void
plymouth_failed (void *             user_data,
		 ply_boot_client_t *client)
{
	nih_error (_("Plymouth command failed"));
}

void
plymouth_answer (void *             user_data,
		 const char *       keys,
		 ply_boot_client_t *client)
{
	Mount *mnt = plymouth_mnt;

	if (! keys)
		return;
	if (plymouth_error == ERROR_NONE)
		return;
	if ((plymouth_mnt != NULL) && (plymouth_mnt->error != plymouth_error))
		return;

	switch (keys[0]) {
	case 'f':
	case 'F':
		if (mnt->error != ERROR_FSCK_FAILED)
			break;
		if (plymouth_error == ERROR_FSCK_IN_PROGRESS)
			break;

		mnt->error = ERROR_NONE;
		plymouth_update (FALSE);

		nih_message (_("Attempting to fix %s filesystem"),
			     MOUNT_NAME (mnt));

		mnt->fsck_fix = TRUE;
		run_fsck (mnt);
		break;
	case 'i':
	case 'I':
		if ((mnt->error != ERROR_FSCK_FAILED)
		    && (mnt->error != ERROR_FSCK_FAILED_HARD))
			break;
		if (plymouth_error == ERROR_FSCK_IN_PROGRESS)
			break;

		mnt->error = ERROR_NONE;
		plymouth_update (FALSE);

		nih_message (_("Ignoring errors with %s at user request"),
			     MOUNT_NAME (mnt));

		mnt->ready = TRUE;
		try_mount (mnt, FALSE);
		break;
	case 's':
	case 'S':
		if (plymouth_error == ERROR_FSCK_IN_PROGRESS)
			break;

		mnt->error = ERROR_NONE;
		plymouth_update (FALSE);

		nih_message (_("Skipping %s at user request"),
			     MOUNT_NAME (mnt));

		skip_mount (mnt);
		break;
	case 'm':
	case 'M':
		if (plymouth_error == ERROR_FSCK_IN_PROGRESS)
			break;

		mnt->error = ERROR_NONE;
		plymouth_update (TRUE);

		nih_message (_("Spawning maintenance shell"));

		if ((mnt->error != ERROR_MOUNT_FAILED)
		    && (! strcmp (mnt->mountpoint, "/"))) {
			exit (EXIT_SHELL_REBOOT);
		} else {
			exit (EXIT_SHELL);
		}
		break;
	case 'c':
	case 'C':
		/* mnt is always NULL since multiple fsck may be in
		 * progress.
		 */
		if (plymouth_error != ERROR_FSCK_IN_PROGRESS)
			break;

		fsck_in_progress = FALSE;
		plymouth_update (FALSE);

		nih_message (_("User cancelled filesystem checks"));

		NIH_LIST_FOREACH (mounts, iter) {
			Mount *mnt = (Mount *)iter;

			if ((mnt->fsck_pid > 0)
			    && (mnt->fsck_progress >= 0))
				kill (mnt->fsck_pid, SIGTERM);
		}
		break;
	default:
		nih_debug ("Received odd keys '%s'", keys);
		break;
	}
}

/*
 * set_dev_wait_time:
 *
 * @option: Not used
 * @arg: String receieved at the mountall command line by nih_option_parser
 * and passed to this function for parsing.
 *
 * This function is used to set the "dev_wait_time option" argument specified
 * at the mountall command line. It is called by the nih_option_parser for
 * parsing the argument string specified by @arg
 *
 */
int
set_dev_wait_time (NihOption *option,
		   const char *arg)
{
        char * end_ptr;
        dev_wait_time = strtol (arg, &end_ptr, 10);
        int err = 0;
        if (dev_wait_time <= 0) {
                nih_error (_("\n Legal values of dev-wait-time lie between 1sec to 2147483647 sec"));
                err = -1;
        }
        else if ((dev_wait_time == LONG_MIN) || (dev_wait_time == LONG_MAX)) {
                nih_error (_("\n Legal values of dev-wait-time lie between 1sec to 2147483647 sec"));
                err = -1;
        }
        else if (*end_ptr != '\0') {
                nih_error (_("\n Legal values of dev-wait-time lie between 1sec to 2147483647 sec"));
                err = -1;
        }
        return err;
}


/**
 * stop_mountpoint_timer:
 * @mountpoint: mountpoint whose timer you want to stop.
 *
 * This function is called to stop a previously started timer of a device
 * Note that for this function to be successful, @mountpoint should be a
 * mountpoint corresponding to a device whose "timeout" option is set in the
 * /etc/fstab.
 *
 * Returns 0 on stopping the timer and -1 on not doing so.
 *
 **/
int
stop_dev_timer (const char *mountpoint)
{
	int ret = -1;

	nih_assert (mountpoint != NULL);

	if (strlen (mountpoint) == 0) {
		nih_message (_("Empty mountpoint specified"));
		return ret;
	}

	NIH_LIST_FOREACH (mounts, iter) {
		Mount *mnt = (Mount *)iter;

		if (mnt->mounted)
			continue;
		if (mnt->tag != TAG_TIMEOUT)
			continue;

                if ((! mnt->ready)
                    && (! mnt->nodev)
                    && (! is_remote (mnt))
                    && ((! strncmp (mnt->device, "/dev/", 5))
                        || (! strncmp (mnt->device, "UUID=", 5))
                        || (! strncmp (mnt->device, "LABEL=", 6)))
                    && (! strcmp (mnt->mountpoint, mountpoint)))
                {
			if (device_ready_timer) {
				nih_debug ("Stopping the timer for the device:"
					"%s for %d seconds, starting timer", 
					MOUNT_NAME (mnt), dev_wait_time);
	                        nih_free (device_ready_timer);
				device_ready_timer = NULL;
				ret = 0;
			}
                        break;

                }
	}
	return ret;
}

/**
 * restart_dev_timer:
 ** @mountpoint: mountpoint whose timer you want to stop.
 *
 * This function is called to restart a previously stopped timer of a device
 * Note that for this function to be successful, @mountpoint should be a
 * mountpoint corresponding to a device whose "timeout" option is set in the
 * /etc/fstab.
 *
 * Returns 0 on restarting the timer and -1 on not doing so.
 *
 **/
int
restart_dev_timer (const char *mountpoint)
{
	int ret = -1;

	nih_assert (mountpoint != NULL);

	if (strlen (mountpoint) == 0) {
		nih_message (_("Empty mountpoint specified"));
		return ret;
	}

	NIH_LIST_FOREACH (mounts, iter) {
		Mount *mnt = (Mount *)iter;

		if (mnt->mounted)
			continue;
		if (mnt->tag != TAG_TIMEOUT)
			continue;

                if ((! mnt->ready)
                    && (! mnt->nodev)
                    && (! is_remote (mnt))
                    && ((! strncmp (mnt->device, "/dev/", 5))
                        || (! strncmp (mnt->device, "UUID=", 5))
                        || (! strncmp (mnt->device, "LABEL=", 6)))
                    && (! strcmp (mnt->mountpoint, mountpoint)))
                {

			if (!dev_wait_time)
				dev_wait_time = ROOTDELAY;
			if (!device_ready_timer) {
				nih_debug ("Shall wait for device: %s for %d "
					"seconds, starting timer", 
					MOUNT_NAME (mnt), dev_wait_time);
				device_ready_timer = NIH_MUST (nih_timer_add_timeout (NULL, 
					dev_wait_time, is_device_ready, NULL));
				ret = 0;
			} 
			break;

                }
	}
	return ret;
}

/**
 * change_mount_device:
 * @devname: name of the new device.
 * @path: full path to a previously existing mountpoint.
 *
 *  This function is called to change the device that would be mounted at a
 *  given mountpoint without calling this function. Note that @path should be
 *  what mountall has read from fstab or what mountall already knows for
 *  mounting a device.
 *
 *  Return 0 on successfully changing the device and -1 on not doing so.
 **/
int
change_mount_device (const char *devname,
	             const char *path)
{
	int ret = -1;

	nih_assert (devname != NULL);
	nih_assert (path != NULL);

	if (strlen (devname) == 0) {
		nih_error(_("Empty device name specified"));
		return ret;
	}
	if (strlen (path) == 0) {
		nih_error (_("Empty mount point specified"));
		return ret;
	}

	NIH_LIST_FOREACH (mounts, iter) {
		Mount *mnt = (Mount *)iter;

		if (mnt->mounted)
			continue;
		if (mnt->tag != TAG_TIMEOUT)
			continue;

                if ((! mnt->ready)
                    && (!mnt->nodev)
                    && (!is_remote (mnt))
                    && ((!strncmp (mnt->device, "/dev/", 5))
                        || (!strncmp (mnt->device, "UUID=", 5))
                        || (!strncmp (mnt->device, "LABEL=", 6)))
                    )
                {
			/* Change the device to be mounted on a pre-registered
			 * mountpoint
			 */
			if ((!strcmp (mnt->mountpoint, path))) {
				/* Change only if the requested device is
				 * really any different than whats stored
				 */
				if (strcmp (mnt->device, devname)) {
					char * newdev;
					ret = 0;
					newdev  = nih_strdup (mounts, devname);
					if (!newdev)  {
						nih_error_raise_no_memory ();
						nih_error (("Could not change the mountpoint "
								"for device: %s"), devname);
						ret = -1;
					}
					else {
						nih_free (mnt->device);
						mnt->device = newdev;
						emit_event ("changed-device", mnt, NULL);
					}
				}
				break;
			}
                }
	}
	return ret;
}

/**
 * options:
 *
 * Command-line options accepted by this program.
 **/
static NihOption options[] = {
	{ 0, "daemon", N_("Detach and run in the background"),
	  NULL, NULL, &daemonise, NULL },
	{ 0, "force-fsck", N_("Force check of all filesystems"),
	  NULL, NULL, &force_fsck, NULL },
	{ 0, "fsck-fix", N_("Attempt to fix all fsck errors"),
	  NULL, NULL, &fsck_fix, NULL },
	{ 0, "no-events", N_("Do not emit events after mounting filesystems"),
	  NULL, NULL, &no_events, NULL },
	{ 0, "dev-wait-time", N_("In case of (bootwait,timeout): specify the time to wait for device to be detected"),
          NULL, N_("value in seconds (Default is 30 seconds, Legal value between 1second to 2147483647 seconds)"), &dev_wait_time, set_dev_wait_time},
	NIH_OPTION_LAST
};


int
main (int   argc,
      char *argv[])
{
	FILE *               cmdline;
	char **              args;
	DBusConnection *     connection;
	struct udev_monitor *udev_monitor;
	Mount *              root;
	int                  ret;

	nih_main_init (argv[0]);

	nih_option_set_synopsis (_("Mount filesystems on boot"));
	nih_option_set_help (
		_("By default, mountall does not detach from the "
		  "console and remains in the foreground.  Use the --daemon "
		  "option to have it detach."));

	args = nih_option_parser (NULL, argc, argv, options, FALSE);
	if (! args)
		exit (EXIT_ERROR);

	nih_signal_reset ();

	/* Initialise the connection to Upstart */
	connection = NIH_SHOULD (nih_dbus_connect (DBUS_ADDRESS_UPSTART, upstart_disconnected));
	if (! connection) {
		NihError *err;

		err = nih_error_get ();
		nih_fatal ("%s: %s", _("Could not connect to Upstart"),
			   err->message);
		nih_free (err);

		exit (EXIT_ERROR);
	}

	upstart = NIH_SHOULD (nih_dbus_proxy_new (NULL, connection,
						  NULL, DBUS_PATH_UPSTART,
						  NULL, NULL));
	if (! upstart) {
		NihError *err;

		err = nih_error_get ();
		nih_fatal ("%s: %s", _("Could not create Upstart proxy"),
			   err->message);
		nih_free (err);

		exit (EXIT_ERROR);
	}

	/* Initialise the connection to udev */
	nih_assert (udev = udev_new ());
	nih_assert (udev_monitor = udev_monitor_new_from_netlink (udev, "udev"));
	nih_assert (udev_monitor_filter_add_match_subsystem_devtype (udev_monitor, "block", NULL) == 0);
	nih_assert (udev_monitor_enable_receiving (udev_monitor) == 0);
	udev_monitor_set_receive_buffer_size (udev_monitor, 128*1024*1024);

	NIH_MUST (nih_io_add_watch (NULL, udev_monitor_get_fd (udev_monitor),
				    NIH_IO_READ,
				    (NihIoWatcher)udev_monitor_watcher,
				    udev_monitor));

	/* Initialise a Plymouth event loop; this is an epoll instance that
	 * we can poll within our own main loop and call out to when needs
	 * be.
	 */
	nih_assert (ply_event_loop = ply_event_loop_new ());

	NIH_MUST (nih_io_add_watch (NULL, *(int *)ply_event_loop,
				    NIH_IO_READ,
				    (NihIoWatcher)ply_event_loop_process_pending_events,
				    ply_event_loop));

	/* Attempt an early connection to Plymouth */
	plymouth_connect ();

	mounts = NIH_MUST (nih_list_new (NULL));

	/* Parse /proc/filesystems to find out which filesystems don't
	 * have devices.
	 */
	parse_filesystems ();

	/* Initialse mount table with built-in filesystems, then parse
	 * from /etc/fstab and /proc/self/mountinfo to find out what else
	 * we need to do.
	 */
	parse_fstab (BUILTIN_FSTAB);
	parse_fstab (_PATH_MNTTAB);
	parse_zfs_list ();
	parse_mountinfo ();

	/* Apply policy as to what waits for what, etc. */
	mount_policy ();

	/* Sanity check, the root filesystem should be already mounted */
	root = find_mount ("/");
	if (! root || !root->mounted ) {
		nih_fatal ("%s", _("root filesystem isn't mounted"));
		exit (EXIT_ERROR);
	}

	/* SIGUSR1 tells us that a network device came up.  Install
           the handler before daemonising so that the mountall-net job
           won't kill us by sending USR1. */
	nih_signal_set_handler (SIGUSR1, nih_signal_handler);
	NIH_MUST (nih_signal_add_handler (NULL, SIGUSR1, usr1_handler, NULL));

	/* Become daemon */
	if (daemonise) {
		pid_t pid;

		fflush (stdout);
		fflush (stderr);

		/* Fork once because Upstart makes us a session leader,
		 * or we may be a session leader of an existing process
		 * group.
		 */
		pid = fork ();
		if (pid < 0) {
			nih_fatal ("%s: %s", _("Unable to become daemon"),
				   strerror (errno));

			exit (EXIT_ERROR);
		} else if (pid > 0) {
			exit (0);
		}

		/* Create a new session */
		setsid ();

		/* Fork again so that we're not the leader of that session */
		pid = fork ();
		if (pid < 0) {
			nih_fatal ("%s: %s", _("Unable to become daemon"),
				   strerror (errno));

			exit (EXIT_ERROR);
		} else if (pid > 0) {
			exit (0);
		}

		/* Usual daemon cleanups */
		if (chdir ("/"))
			;
		umask (0);

		/* Send all logging output to syslog */
		//openlog (program_name, LOG_PID, LOG_DAEMON);
		//nih_log_set_logger (nih_logger_syslog);

		nih_signal_set_ignore (SIGHUP);
	}

	nih_signal_set_handler (SIGCHLD, nih_signal_handler);

	/* Handle TERM signal gracefully */
	nih_signal_set_handler (SIGTERM, nih_signal_handler);
	NIH_MUST (nih_signal_add_handler (NULL, SIGTERM, nih_main_term_signal, NULL));

	/* Check for force-fsck on the kernel command line */
	cmdline = fopen ("/proc/cmdline", "r");
	if (cmdline) {
		char line[4096];

		if (fgets (line, sizeof line, cmdline)) {
			char *tok;

			for (tok = strtok (line, " "); tok;
			     tok = strtok (NULL, " "))
				if (! strcmp (tok, "force-fsck"))
					force_fsck = 1;
		}

		fclose (cmdline);
	}

	/* See what's already mounted */
	mark_mounted ();
	/* Create a listening server for private connections. */
	while ((ret = control_server_open ()) < 0) {
		NihError *err;

		err = nih_error_get ();
		if (err->number != ENOMEM) {
			nih_warn ("%s: %s", _("Unable to listen for private"
						"connections"), err->message);
			nih_free (err);
			break;
		}
		nih_free (err);
	}
	if (!ret)
		emit_event("mountallServer", NULL, NULL);
	/* Activate the timer for a fs that is local, unmounted and waits for
	 * a device to be ready, before it can be mounted onto it. Timer on
	 * only for fs not marked with a "nobootwait=1" */
	activate_timer();

	/* See what we can mount straight away, and then schedule the same
	 * function to be run each time through the main loop.
	 */
	try_mounts ();
	NIH_MUST (nih_main_loop_add_func (NULL, (NihMainLoopCb)try_mounts, NULL));

	/* Catch up with udev */
	udev_catchup ();

	ret = nih_main_loop ();

	/* Flush the D-Bus connection to ensure all messages are sent */
	dbus_connection_flush (connection);

	/* Flush the Plymouth connection to ensure all updates are sent */
	if (ply_boot_client)
		ply_boot_client_flush (ply_boot_client);

	return ret;
}


void
usr1_handler (void *     data,
	      NihSignal *signal)
{
	nih_info (_("Received SIGUSR1 (network device up)"));

	newly_mounted = TRUE;
	nih_main_loop_interrupt ();
}
