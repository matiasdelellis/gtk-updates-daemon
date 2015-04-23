
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

#include "gud-sync-helper.h"

/* tiny helper to help us do the async operation */
typedef struct {
	GError    **error;
	GMainLoop  *loop;
	guint       seconds;
} GudTaskHelper;

static void
gud_control_get_time_since_action_sync_finished (GObject      *object,
                                                 GAsyncResult *res,
                                                 gpointer      data)
{
	GudTaskHelper *helper = data;

	/* get the result */
	helper->seconds =
		pk_control_get_time_since_action_finish (PK_CONTROL(object), res, helper->error);

	g_main_loop_quit (helper->loop);
}

guint
gud_control_get_time_since_action_sync (PkControl     *control,
                                        PkRoleEnum     role,
                                        GCancellable  *cancellable,
                                        GError       **error)
{
	GudTaskHelper *helper;
	guint seconds = -1;

	g_return_val_if_fail (PK_CONTROL (control), -1);
	g_return_val_if_fail (error == NULL || *error == NULL, -1);

	/* create temp object */
	helper = g_new0 (GudTaskHelper, 1);
	helper->loop = g_main_loop_new (NULL, FALSE);
	helper->error = error;

	/* run async method */

	pk_control_get_time_since_action_async (control,
	                                        role,
	                                        cancellable,
	                                        (GAsyncReadyCallback) gud_control_get_time_since_action_sync_finished,
	                                        helper);

	g_main_loop_run (helper->loop);

	seconds = helper->seconds;

	/* free temp object */
	g_main_loop_unref (helper->loop);
	g_free (helper);

	return seconds;
}
