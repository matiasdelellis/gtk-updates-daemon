/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2009 Richard Hughes <richard@hughsie.com>
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

#ifndef __GUD_PK_PROGRESS_BAR_H
#define __GUD_PK_PROGRESS_BAR_H

#include <glib-object.h>

G_BEGIN_DECLS

#define GUD_PK_TYPE_PROGRESS_BAR		(gud_pk_progress_bar_get_type ())
#define GUD_PK_PROGRESS_BAR(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GUD_PK_TYPE_PROGRESS_BAR, GudPkProgressBar))
#define GUD_PK_PROGRESS_BAR_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GUD_PK_TYPE_PROGRESS_BAR, GudPkProgressBarClass))
#define GUD_PK_IS_PROGRESS_BAR(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GUD_PK_TYPE_PROGRESS_BAR))
#define GUD_PK_IS_PROGRESS_BAR_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GUD_PK_TYPE_PROGRESS_BAR))
#define GUD_PK_PROGRESS_BAR_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GUD_PK_TYPE_PROGRESS_BAR, GudPkProgressBarClass))

typedef struct GudPkProgressBarPrivate GudPkProgressBarPrivate;

typedef struct
{
	GObject			 parent;
	GudPkProgressBarPrivate	*priv;
} GudPkProgressBar;

typedef struct
{
	GObjectClass		 parent_class;
} GudPkProgressBarClass;

GType		 gud_pk_progress_bar_get_type		(void);
GudPkProgressBar	*gud_pk_progress_bar_new			(void);
gboolean	 gud_pk_progress_bar_set_size		(GudPkProgressBar	*progress_bar,
							 guint		 size);
gboolean	 gud_pk_progress_bar_set_padding		(GudPkProgressBar	*progress_bar,
							 guint		 padding);
gboolean	 gud_pk_progress_bar_set_percentage		(GudPkProgressBar	*progress_bar,
							 gint		 percentage);
gboolean	 gud_pk_progress_bar_start			(GudPkProgressBar	*progress_bar,
							 const gchar	*text);
gboolean	 gud_pk_progress_bar_end			(GudPkProgressBar	*progress_bar);

G_END_DECLS

#endif /* __GUD_PK_PROGRESS_BAR_H */
