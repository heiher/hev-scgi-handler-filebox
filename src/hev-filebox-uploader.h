/*
 ============================================================================
 Name        : hev-filebox-uploader.h
 Author      : Heiher <root@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2013 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_FILEBOX_UPLOADER_H__
#define __HEV_FILEBOX_UPLOADER_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define HEV_TYPE_FILEBOX_UPLOADER (hev_filebox_uploader_get_type ())
#define HEV_FILEBOX_UPLOADER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
				HEV_TYPE_FILEBOX_UPLOADER, HevFileboxUploader))
#define HEV_IS_FILEBOX_UPLOADER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
				HEV_TYPE_FILEBOX_UPLOADER))
#define HEV_FILEBOX_UPLOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), \
				HEV_TYPE_FILEBOX_UPLOADER, HevFileboxUploaderClass))
#define HEV_IS_FILEBOX_UPLOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
				HEV_TYPE_FILEBOX_UPLOADER))
#define HEV_FILEBOX_UPLOADER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), \
				HEV_TYPE_FILEBOX_UPLOADER, HevFileboxUploaderClass))

typedef struct _HevFileboxUploader HevFileboxUploader;
typedef struct _HevFileboxUploaderClass HevFileboxUploaderClass;

struct _HevFileboxUploader
{
	GObject parent_instance;
};

struct _HevFileboxUploaderClass
{
	GObjectClass parent_class;
};

GType hev_filebox_uploader_get_type (void);

GObject * hev_filebox_uploader_new (GKeyFile *config);

void hev_filebox_uploader_handle_async (HevFileboxUploader *self, GObject *scgi_task,
			GAsyncReadyCallback callback, gpointer user_data);
gboolean hev_filebox_uploader_handle_finish (HevFileboxUploader *self,
			GAsyncResult *result, GError **error);

G_END_DECLS

#endif /* __HEV_FILEBOX_UPLOADER_H__ */

