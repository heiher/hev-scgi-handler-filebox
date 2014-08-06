/*
 ============================================================================
 Name        : hev-filebox-deleter.h
 Author      : Heiher <root@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2013 everyone.
 Description :
 ============================================================================
 */

#ifndef __HEV_FILEBOX_DELETER_H__
#define __HEV_FILEBOX_DELETER_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define HEV_TYPE_FILEBOX_DELETER (hev_filebox_deleter_get_type ())
#define HEV_FILEBOX_DELETER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                HEV_TYPE_FILEBOX_DELETER, HevFileboxDeleter))
#define HEV_IS_FILEBOX_DELETER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                HEV_TYPE_FILEBOX_DELETER))
#define HEV_FILEBOX_DELETER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), \
                HEV_TYPE_FILEBOX_DELETER, HevFileboxDeleterClass))
#define HEV_IS_FILEBOX_DELETER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                HEV_TYPE_FILEBOX_DELETER))
#define HEV_FILEBOX_DELETER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                HEV_TYPE_FILEBOX_DELETER, HevFileboxDeleterClass))

typedef struct _HevFileboxDeleter HevFileboxDeleter;
typedef struct _HevFileboxDeleterClass HevFileboxDeleterClass;

struct _HevFileboxDeleter
{
    GObject parent_instance;
};

struct _HevFileboxDeleterClass
{
    GObjectClass parent_class;
};

GType hev_filebox_deleter_get_type (void);

GObject * hev_filebox_deleter_new (GKeyFile *config);

void hev_filebox_deleter_handle_async (HevFileboxDeleter *self, GObject *scgi_task,
			GAsyncReadyCallback callback, gpointer user_data);
gboolean hev_filebox_deleter_handle_finish (HevFileboxDeleter *self,
			GAsyncResult *result, GError **error);

G_END_DECLS

#endif /* __HEV_FILEBOX_DELETER_H__ */

