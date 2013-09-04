/*
 ============================================================================
 Name        : hev-filebox-downloader.h
 Author      : Heiher <root@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2013 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_FILEBOX_DOWNLOADER_H__
#define __HEV_FILEBOX_DOWNLOADER_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define HEV_TYPE_FILEBOX_DOWNLOADER (hev_filebox_downloader_get_type ())
#define HEV_FILEBOX_DOWNLOADER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                HEV_TYPE_FILEBOX_DOWNLOADER, HevFileboxDownloader))
#define HEV_IS_FILEBOX_DOWNLOADER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                HEV_TYPE_FILEBOX_DOWNLOADER))
#define HEV_FILEBOX_DOWNLOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), \
                HEV_TYPE_FILEBOX_DOWNLOADER, HevFileboxDownloaderClass))
#define HEV_IS_FILEBOX_DOWNLOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                HEV_TYPE_FILEBOX_DOWNLOADER))
#define HEV_FILEBOX_DOWNLOADER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                HEV_TYPE_FILEBOX_DOWNLOADER, HevFileboxDownloaderClass))

typedef struct _HevFileboxDownloader HevFileboxDownloader;
typedef struct _HevFileboxDownloaderClass HevFileboxDownloaderClass;

struct _HevFileboxDownloader
{
    GObject parent_instance;
};

struct _HevFileboxDownloaderClass
{
    GObjectClass parent_class;
};

GType hev_filebox_downloader_get_type (void);

GObject * hev_filebox_downloader_new (GKeyFile *config);

void hev_filebox_downloader_handle_async (HevFileboxDownloader *self, GObject *scgi_task,
			GAsyncReadyCallback callback, gpointer user_data);
gboolean hev_filebox_downloader_handle_finish (HevFileboxDownloader *self,
			GAsyncResult *result, GError **error);

G_END_DECLS

#endif /* __HEV_FILEBOX_DOWNLOADER_H__ */

