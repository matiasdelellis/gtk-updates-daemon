/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2008-2009 Richard Hughes <richard@hughsie.com>
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

/*
 * NOTE: This is a cnp file from packagekit glib lib.
 * Just add the prefix 'gdu' to prevent any posibles errors.
 */

#include <glib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "gud-pk-progress-bar.h"

#define GUD_PK_PROGRESS_BAR_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GUD_PK_TYPE_PROGRESS_BAR, GudPkProgressBarPrivate))

typedef struct {
	guint			 position;
	gboolean		 move_forward;
} GudPkProgressBarPulseState;

struct GudPkProgressBarPrivate
{
	guint			 size;
	gint			 percentage;
	guint			 padding;
	guint			 timer_id;
	GudPkProgressBarPulseState	 pulse_state;
	gint			 tty_fd;
	gchar			*old_start_text;
};

#define GUD_PK_PROGRESS_BAR_PERCENTAGE_INVALID	101
#define GUD_PK_PROGRESS_BAR_PULSE_TIMEOUT		40 /* ms */

G_DEFINE_TYPE (GudPkProgressBar, gud_pk_progress_bar, G_TYPE_OBJECT)

/**
 * gud_pk_progress_bar_console:
 **/
static void
gud_pk_progress_bar_console (GudPkProgressBar *self, const gchar *tmp)
{
	gssize count;
	gssize wrote;
	count = strlen (tmp) + 1;
	wrote = write (self->priv->tty_fd, tmp, count);
	if (wrote != count) {
		g_warning ("Only wrote %" G_GSSIZE_FORMAT
			   " of %" G_GSSIZE_FORMAT " bytes",
			   wrote, count);
	}
}

/**
 * gud_pk_progress_bar_set_padding:
 **/
gboolean
gud_pk_progress_bar_set_padding (GudPkProgressBar *self, guint padding)
{
	g_return_val_if_fail (GUD_PK_IS_PROGRESS_BAR (self), FALSE);
	g_return_val_if_fail (padding < 100, FALSE);
	self->priv->padding = padding;
	return TRUE;
}

/**
 * gud_pk_progress_bar_set_size:
 **/
gboolean
gud_pk_progress_bar_set_size (GudPkProgressBar *self, guint size)
{
	g_return_val_if_fail (GUD_PK_IS_PROGRESS_BAR (self), FALSE);
	g_return_val_if_fail (size < 100, FALSE);
	self->priv->size = size;
	return TRUE;
}

/**
 * gud_pk_progress_bar_draw:
 **/
static gboolean
gud_pk_progress_bar_draw (GudPkProgressBar *self, gint percentage)
{
	guint section;
	guint i;
	GString *str;

	/* no value yet */
	if (percentage == G_MININT)
		return FALSE;

	/* restore cursor */
	str = g_string_new ("");
	g_string_append_printf (str, "%c8", 0x1B);

	section = (guint) ((gfloat) self->priv->size / (gfloat) 100.0 * (gfloat) percentage);
	g_string_append (str, "[");
	for (i = 0; i < section; i++)
		g_string_append (str, "=");
	for (i = 0; i < self->priv->size - section; i++)
		g_string_append (str, " ");
	g_string_append (str, "] ");
	if (percentage >= 0 && percentage < 100)
		g_string_append_printf (str, "(%i%%)  ", percentage);
	else
		g_string_append (str, "        ");
	gud_pk_progress_bar_console (self, str->str);
	g_string_free (str, TRUE);
	return TRUE;
}

/**
 * gud_pk_progress_bar_pulse_bar:
 **/
static gboolean
gud_pk_progress_bar_pulse_bar (GudPkProgressBar *self)
{
	gint i;
	GString *str;

	/* restore cursor */
	str = g_string_new ("");
	g_string_append_printf (str, "%c8", 0x1B);

	if (self->priv->pulse_state.move_forward) {
		if (self->priv->pulse_state.position == self->priv->size - 1)
			self->priv->pulse_state.move_forward = FALSE;
		else
			self->priv->pulse_state.position++;
	} else if (!self->priv->pulse_state.move_forward) {
		if (self->priv->pulse_state.position == 1)
			self->priv->pulse_state.move_forward = TRUE;
		else
			self->priv->pulse_state.position--;
	}

	g_string_append (str, "[");
	for (i = 0; i < (gint)self->priv->pulse_state.position-1; i++)
		g_string_append (str, " ");
	g_string_append (str, "==");
	for (i = 0; i < (gint) (self->priv->size - self->priv->pulse_state.position - 1); i++)
		g_string_append (str, " ");
	g_string_append (str, "] ");
	if (self->priv->percentage >= 0 && self->priv->percentage != GUD_PK_PROGRESS_BAR_PERCENTAGE_INVALID)
		g_string_append_printf (str, "(%i%%)  ", self->priv->percentage);
	else
		g_string_append (str, "        ");
	gud_pk_progress_bar_console (self, str->str);
	g_string_free (str, TRUE);

	return TRUE;
}

/**
 * gud_pk_progress_bar_draw_pulse_bar:
 **/
static void
gud_pk_progress_bar_draw_pulse_bar (GudPkProgressBar *self)
{
	/* have we already got zero percent? */
	if (self->priv->timer_id != 0)
		return;
	if (TRUE) {
		self->priv->pulse_state.position = 1;
		self->priv->pulse_state.move_forward = TRUE;
		self->priv->timer_id = g_timeout_add (GUD_PK_PROGRESS_BAR_PULSE_TIMEOUT,
						      (GSourceFunc) gud_pk_progress_bar_pulse_bar, self);
		g_source_set_name_by_id (self->priv->timer_id, "[GudPkProgressBar] pulse");
	}
}

/**
 * gud_pk_progress_bar_set_percentage:
 **/
gboolean
gud_pk_progress_bar_set_percentage (GudPkProgressBar *self, gint percentage)
{
	g_return_val_if_fail (GUD_PK_IS_PROGRESS_BAR (self), FALSE);
	g_return_val_if_fail (percentage <= GUD_PK_PROGRESS_BAR_PERCENTAGE_INVALID, FALSE);

	/* never called gud_pk_progress_bar_start() */
	if (self->priv->percentage == G_MININT)
		gud_pk_progress_bar_start (self, "FIXME: need to call gud_pk_progress_bar_start() earlier!");

	/* check for old percentage */
	if (percentage == self->priv->percentage) {
		g_debug ("skipping as the same");
		goto out;
	}

	/* save */
	self->priv->percentage = percentage;

	/* either pulse or display */
	if (percentage < 0 || percentage > 100) {
		gud_pk_progress_bar_draw (self, 0);
		gud_pk_progress_bar_draw_pulse_bar (self);
	} else {
		if (self->priv->timer_id != 0) {
			g_source_remove (self->priv->timer_id);
			self->priv->timer_id = 0;
		}
		gud_pk_progress_bar_draw (self, percentage);
	}
out:
	return TRUE;
}

/**
 * pk_strpad:
 * @data: the input string
 * @length: the desired length of the output string, with padding
 *
 * Returns the text padded to a length with spaces. If the string is
 * longer than length then a longer string is returned.
 *
 * Return value: The padded string
 **/
static gchar *
pk_strpad (const gchar *data, guint length)
{
	gint size;
	guint data_len;
	gchar *text;
	gchar *padding;

	if (data == NULL)
		return g_strnfill (length, ' ');

	/* ITS4: ignore, only used for formatting */
	data_len = strlen (data);

	/* calculate */
	size = (length - data_len);
	if (size <= 0)
		return g_strdup (data);

	padding = g_strnfill (size, ' ');
	text = g_strdup_printf ("%s%s", data, padding);
	g_free (padding);
	return text;
}

/**
 * gud_pk_progress_bar_start:
 **/
gboolean
gud_pk_progress_bar_start (GudPkProgressBar *self, const gchar *text)
{
	gchar *text_pad;
	GString *str;

	g_return_val_if_fail (GUD_PK_IS_PROGRESS_BAR (self), FALSE);

	/* same as last time */
	if (g_strcmp0 (self->priv->old_start_text, text) == 0)
		return TRUE;
	g_free (self->priv->old_start_text);
	self->priv->old_start_text = g_strdup (text);

	/* finish old value */
	str = g_string_new ("");
	if (self->priv->percentage != G_MININT) {
		gud_pk_progress_bar_draw (self, 100);
		g_string_append (str, "\n");
	}

	/* make these all the same length */
	text_pad = pk_strpad (text, self->priv->padding);
	g_string_append (str, text_pad);

	/* save cursor in new position */
	g_string_append_printf (str, "%c7", 0x1B);
	gud_pk_progress_bar_console (self, str->str);

	/* reset */
	if (self->priv->percentage == G_MININT)
		self->priv->percentage = 0;
	gud_pk_progress_bar_draw (self, 0);

	g_string_free (str, TRUE);
	g_free (text_pad);
	return TRUE;
}

/**
 * gud_pk_progress_bar_end:
 **/
gboolean
gud_pk_progress_bar_end (GudPkProgressBar *self)
{
	GString *str;

	g_return_val_if_fail (GUD_PK_IS_PROGRESS_BAR (self), FALSE);

	/* never drawn */
	if (self->priv->percentage == G_MININT)
		return FALSE;

	self->priv->percentage = G_MININT;
	gud_pk_progress_bar_draw (self, 100);
	str = g_string_new ("");
	g_string_append_printf (str, "\n");
	gud_pk_progress_bar_console (self, str->str);
	g_string_free (str, TRUE);

	return TRUE;
}

/**
 * gud_pk_progress_bar_finalize:
 **/
static void
gud_pk_progress_bar_finalize (GObject *object)
{
	GudPkProgressBar *self;
	g_return_if_fail (GUD_PK_IS_PROGRESS_BAR (object));
	self = GUD_PK_PROGRESS_BAR (object);

	g_free (self->priv->old_start_text);
	if (self->priv->timer_id != 0)
		g_source_remove (self->priv->timer_id);
	if (self->priv->tty_fd > 0)
		close (self->priv->tty_fd);
	G_OBJECT_CLASS (gud_pk_progress_bar_parent_class)->finalize (object);
}

/**
 * gud_pk_progress_bar_class_init:
 **/
static void
gud_pk_progress_bar_class_init (GudPkProgressBarClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = gud_pk_progress_bar_finalize;
	g_type_class_add_private (klass, sizeof (GudPkProgressBarPrivate));
}

/**
 * gud_pk_progress_bar_init:
 **/
static void
gud_pk_progress_bar_init (GudPkProgressBar *self)
{
	self->priv = GUD_PK_PROGRESS_BAR_GET_PRIVATE (self);

	self->priv->size = 10;
	self->priv->percentage = G_MININT;
	self->priv->padding = 0;
	self->priv->timer_id = 0;
	self->priv->tty_fd = open ("/dev/tty", O_RDWR, 0);
	if (self->priv->tty_fd < 0)
		self->priv->tty_fd = open ("/dev/console", O_RDWR, 0);
	if (self->priv->tty_fd < 0)
		self->priv->tty_fd = open ("/dev/stdout", O_RDWR, 0);
}

/**
 * gud_pk_progress_bar_new:
 *
 * Return value: A new progress_bar class instance.
 **/
GudPkProgressBar *
gud_pk_progress_bar_new (void)
{
	GudPkProgressBar *self;
	self = g_object_new (GUD_PK_TYPE_PROGRESS_BAR, NULL);

	if (self->priv->tty_fd < 0) {
		g_object_unref (self);
		self = NULL;
	}

	return self;
}
