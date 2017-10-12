/*
 ============================================================================
 Name        : hev-filebox-querier.h
 Author      : Heiher <root@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2013 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_FILEBOX_QUERIER_H__
#define __HEV_FILEBOX_QUERIER_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define HEV_TYPE_FILEBOX_QUERIER (hev_filebox_querier_get_type ())
#define HEV_FILEBOX_QUERIER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
				HEV_TYPE_FILEBOX_QUERIER, HevFileboxQuerier))
#define HEV_IS_FILEBOX_QUERIER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
				HEV_TYPE_FILEBOX_QUERIER))
#define HEV_FILEBOX_QUERIER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), \
				HEV_TYPE_FILEBOX_QUERIER, HevFileboxQuerierClass))
#define HEV_IS_FILEBOX_QUERIER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
				HEV_TYPE_FILEBOX_QUERIER))
#define HEV_FILEBOX_QUERIER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), \
				HEV_TYPE_FILEBOX_QUERIER, HevFileboxQuerierClass))

typedef struct _HevFileboxQuerier HevFileboxQuerier;
typedef struct _HevFileboxQuerierClass HevFileboxQuerierClass;

struct _HevFileboxQuerier
{
	GObject parent_instance;
};

struct _HevFileboxQuerierClass
{
	GObjectClass parent_class;
};

GType hev_filebox_querier_get_type (void);

GObject * hev_filebox_querier_new (GKeyFile *config);

void hev_filebox_querier_handle_async (HevFileboxQuerier *self, GObject *scgi_task,
			GAsyncReadyCallback callback, gpointer user_data);
gboolean hev_filebox_querier_handle_finish (HevFileboxQuerier *self,
			GAsyncResult *result, GError **error);

G_END_DECLS

#endif /* __HEV_FILEBOX_QUERIER_H__ */

