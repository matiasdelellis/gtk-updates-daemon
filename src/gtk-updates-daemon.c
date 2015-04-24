/*************************************************************************/
/* Copyright (C) 2015 matias <mati86dl@gmail.com>                        */
/*                                                                       */
/* This program is free software: you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation, either version 3 of the License, or     */
/* (at your option) any later version.                                   */
/*                                                                       */
/* This program is distributed in the hope that it will be useful,       */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/* GNU General Public License for more details.                          */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */
/*************************************************************************/

#define I_KNOW_THE_PACKAGEKIT_GLIB2_API_IS_SUBJECT_TO_CHANGE

#include <gtk/gtk.h>
#include <packagekit-glib2/packagekit.h>
#include <libnotify/notify.h>

#include "gud-sync-helper.h"
#include "gud-pk-progress-bar.h"

#define TRESHOLD 3600
#define BINDIR "/usr/bin"

#define _(x) x

static GudPkProgressBar *progressbar = NULL;
static GCancellable *cancellable = NULL;

static void
gud_console_progress_cb (PkProgress *progress, PkProgressType type, gpointer data);

/*
 * Notificacions
 */
static void
on_notification_closed (NotifyNotification *notification, gpointer data)
{
	g_object_unref (notification);
	gtk_main_quit ();
}

static void
libnotify_action_cb (NotifyNotification *notification,
                     gchar              *action,
                     gpointer            user_data)
{
	gboolean ret;
	GError *error = NULL;

	if (g_strcmp0 (action, "show-update-viewer") == 0) {
		ret = g_spawn_command_line_async (BINDIR "/gpk-update-viewer",
		                                  &error);
		if (!ret) {
			g_warning ("Failure launching update viewer: %s",
			           error->message);
			g_error_free (error);
		}
	}

	notify_notification_close (notification, NULL);
}

static void
gud_notfy_updates (GPtrArray *packages)
{
	const gchar *title;
	gboolean ret;
	GError *error = NULL;
	GString *string = NULL;
	guint i;
	NotifyNotification *notification;
	PkPackage *item;

	/* find the upgrade string */
	string = g_string_new ("");
	for (i=0; i < packages->len; i++) {
		item = (PkPackage *) g_ptr_array_index (packages, i);
		g_string_append_printf (string, "%s (%s)\n",
		                        pk_package_get_name (item),
		                        pk_package_get_version(item));
	}

	if (string->len != 0)
		g_string_set_size (string, string->len-1);

	title = _("Distribution updates available");
	notification = notify_notification_new (title,
	                                        string->str,
	                                        "software-update-available");

	notify_notification_set_hint_string (notification, "desktop-entry", "gtk-updates-daemon");
	notify_notification_set_app_name (notification, _("Software Updates"));
	notify_notification_set_timeout (notification, NOTIFY_EXPIRES_NEVER);
	notify_notification_set_urgency (notification, NOTIFY_URGENCY_NORMAL);

	notify_notification_add_action (notification, "ignore",
	                                /* TRANSLATORS: don't install updates now */
	                                _("Not Now"),
	                                libnotify_action_cb,
	                                NULL, NULL);
	notify_notification_add_action (notification, "show-update-viewer",
	                                /* TRANSLATORS: button: open the update viewer to install updates */
	                                _("Install updates"),
	                                libnotify_action_cb,
	                                NULL, NULL);

	g_signal_connect (notification, "closed",
	                  G_CALLBACK (on_notification_closed), NULL);

	ret = notify_notification_show (notification, &error);
	if (!ret) {
		g_warning ("error: %s", error->message);
		g_error_free (error);
		gtk_main_quit ();
	}
}

/*
 * Check updates.
 */
static void
gud_check_updates_finished (GObject      *object,
                            GAsyncResult *res,
                            gpointer      data)
{
	GError *error = NULL;
	GPtrArray *array = NULL;
	GString *string = NULL;
	PkClient *client = PK_CLIENT(object);
	PkError *error_code = NULL;
	PkResults *results;

	gud_pk_progress_bar_end (progressbar);

	/* get the results */
	results = pk_client_generic_finish (PK_CLIENT(client), res, &error);
	if (results == NULL) {
		if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED)) {
			g_debug ("Query updates was cancelled");
			g_error_free (error);
			return;
		}
		if (error->domain != PK_CLIENT_ERROR ||
			error->code != PK_CLIENT_ERROR_NOT_SUPPORTED) {
			g_warning ("failed to get upgrades: %s",
			           error->message);
		}
		g_error_free (error);
		goto out;
	}

	/* check error code */
	error_code = pk_results_get_error_code (results);
	if (error_code != NULL) {
		g_warning ("failed to get upgrades: %s, %s",
		           pk_error_enum_to_string (pk_error_get_code (error_code)),
		           pk_error_get_details (error_code));
		goto out;
	}

	/* process results */
	array = pk_results_get_package_array(results);

	/* have updates? */
	if (array->len > 0) {
		gud_notfy_updates (array);
	}
	else {
		g_message ("no upgrades\n");
		gtk_main_quit ();
	}

out:
	if (error_code != NULL)
		g_object_unref (error_code);
	if (array != NULL)
		g_ptr_array_unref (array);
	if (string != NULL)
		g_string_free (string, TRUE);
	if (results != NULL)
		g_object_unref (results);
}

static void
gud_check_updates (PkTask *task)
{
	/* get new update list */

	g_debug ("Checking updates");

	pk_client_get_updates_async (PK_CLIENT(task),
	                             pk_bitfield_value (PK_FILTER_ENUM_NONE),
	                             cancellable,
	                             (PkProgressCallback) gud_console_progress_cb, NULL,
	                             (GAsyncReadyCallback) gud_check_updates_finished,
	                             task);
}

/*
 * Refresh package database
 */
static void
gud_refresh_package_cache_finished (GObject      *object,
                                    GAsyncResult *res,
                                    gpointer      data)
{
	PkResults *results;
	GError *error = NULL;
	PkError *error_code = NULL;

	PkTask *task = PK_TASK(data);

	gud_pk_progress_bar_end (progressbar);

	/* get the results */
	results = pk_client_generic_finish (PK_CLIENT(object), res, &error);
	if (results == NULL) {
		if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED)) {
			g_debug ("Refresh database was cancelled");
			g_error_free (error);
			return;
		}
		g_warning ("failed to refresh the cache: %s", error->message);
		g_error_free (error);
		return;
	}

	/* check error code */
	error_code = pk_results_get_error_code (results);
	if (error_code != NULL) {
		g_warning ("failed to refresh the cache: %s, %s",
		           pk_error_enum_to_string (pk_error_get_code (error_code)),
		           pk_error_get_details (error_code));

		goto out;
	}

	g_debug ("Refresh package database finished");

	gud_check_updates (task);

out:
	if (error_code != NULL)
		g_object_unref (error_code);
	if (results != NULL)
		g_object_unref (results);
}

static void
gud_refresh_package_cache (PkTask *task)
{
	g_debug ("Refresh package database");

	pk_client_refresh_cache_async (PK_CLIENT(task),
	                               TRUE,
	                               cancellable,
	                               (PkProgressCallback) gud_console_progress_cb, NULL,
	                               (GAsyncReadyCallback) gud_refresh_package_cache_finished,
	                                task);
}

/*
 * Helper to print progress to console.
 * Based on: https://github.com/hughsie/PackageKit/blob/master/client/pk-console.c
 */

static void
gud_console_progress_cb (PkProgress *progress, PkProgressType type, gpointer data)
{
	gint percentage;
	PkStatusEnum status;
	PkRoleEnum role;
	const gchar *text;

	/* role */
	if (type == PK_PROGRESS_TYPE_ROLE) {
		g_object_get (progress,
		              "role", &role,
		              NULL);
		if (role == PK_ROLE_ENUM_UNKNOWN)
			return;
		/* show new status on the bar */
		text = pk_role_enum_to_localised_present (role);
		gud_pk_progress_bar_start (progressbar, text);
	}

	/* percentage */
	if (type == PK_PROGRESS_TYPE_PERCENTAGE) {
		g_object_get (progress,
		              "percentage", &percentage,
		              NULL);
		gud_pk_progress_bar_set_percentage (progressbar, percentage);
	}
}

/*
 * Main code.
 */

static void
gud_console_sigint_cb (int sig)
{
	g_debug ("Handling SIGINT");

	g_cancellable_cancel (cancellable);
}

int
main (int   argc,
      char *argv[])
{
	PkControl *control = NULL;
	PkTask *task = NULL;
	GError *error = NULL;
	guint seconds;

	notify_init ("gtk-updates-daemon");

	gtk_init (&argc, &argv);

	/* Minimun package kit init. */

	control = pk_control_new ();

	task = pk_task_new ();
	g_object_set (task,
	              "background", TRUE,
	              "interactive", FALSE,
	              "only-download", TRUE,
	              NULL);

	/* Cancel wit ctl+c */

	signal (SIGINT, gud_console_sigint_cb);
	cancellable = g_cancellable_new ();

	/* Helper to print progress on console. */

	progressbar = gud_pk_progress_bar_new ();
	gud_pk_progress_bar_set_size (progressbar, 60);
	gud_pk_progress_bar_set_padding (progressbar, 30);

	/* get the time since the last refresh */

	seconds = gud_control_get_time_since_action_sync (control,
	                                                  PK_ROLE_ENUM_REFRESH_CACHE,
	                                                  cancellable,
	                                                  &error);

	if (seconds == 0 || error != NULL) {
		g_warning ("failed to get time: %s", error->message);
		g_error_free (error);
		return;
	}

	if (seconds > TRESHOLD) {
		gud_refresh_package_cache (task);
	}
	else {
		gud_check_updates (task);
	}

	/* Main loop */

	gtk_main ();

	/* Close main loop. */

	g_object_unref (control);
	g_object_unref (task);

	g_object_unref (cancellable);

	if (progressbar != NULL)
		g_object_unref (progressbar);

	notify_uninit ();

	return 0;
}
