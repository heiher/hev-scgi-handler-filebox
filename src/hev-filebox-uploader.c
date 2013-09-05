/*
 ============================================================================
 Name        : hev-filebox-uploader.c
 Author      : Heiher <root@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2013 everyone.
 Description : 
 ============================================================================
 */

#include <hev-scgi-1.0.h>

#include "hev-filebox-uploader.h"

enum
{
	PROP_0,
	PROP_CONFIG,
	N_PROPERTIES
};

static GParamSpec *hev_filebox_uploader_properties[N_PROPERTIES] = { NULL };

#define HEV_FILEBOX_UPLOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                HEV_TYPE_FILEBOX_UPLOADER, HevFileboxUploaderPrivate))

typedef struct _HevFileboxUploaderPrivate HevFileboxUploaderPrivate;

struct _HevFileboxUploaderPrivate
{
	GKeyFile *config;
};

static void filebox_uploader_handle_task_handler (GTask *task, gpointer source_object,
			gpointer task_data, GCancellable *cancellable);

G_DEFINE_TYPE (HevFileboxUploader, hev_filebox_uploader, G_TYPE_OBJECT);

static void
hev_filebox_uploader_dispose (GObject *obj)
{
    HevFileboxUploader *self = HEV_FILEBOX_UPLOADER (obj);
    HevFileboxUploaderPrivate *priv = HEV_FILEBOX_UPLOADER_GET_PRIVATE (self);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    G_OBJECT_CLASS (hev_filebox_uploader_parent_class)->dispose (obj);
}

static void
hev_filebox_uploader_finalize (GObject *obj)
{
    HevFileboxUploader *self = HEV_FILEBOX_UPLOADER (obj);
    HevFileboxUploaderPrivate *priv = HEV_FILEBOX_UPLOADER_GET_PRIVATE (self);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->config) {
		g_key_file_unref (priv->config);
		priv->config = NULL;
	}

    G_OBJECT_CLASS (hev_filebox_uploader_parent_class)->finalize (obj);
}

static GObject *
hev_filebox_uploader_constructor (GType type,
            guint n,
            GObjectConstructParam *param)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    return G_OBJECT_CLASS (hev_filebox_uploader_parent_class)->
        constructor (type, n, param);
}

static void
hev_filebox_uploader_constructed (GObject *obj)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    G_OBJECT_CLASS (hev_filebox_uploader_parent_class)->constructed (obj);
}

static void
hev_filebox_uploader_set_property(GObject *obj,
			guint prop_id, const GValue *value, GParamSpec *pspec)
{
	HevFileboxUploader *self = HEV_FILEBOX_UPLOADER(obj);
	HevFileboxUploaderPrivate *priv = HEV_FILEBOX_UPLOADER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	switch(prop_id) {
	case PROP_CONFIG:
			priv->config = g_key_file_ref (g_value_get_pointer(value));
			break;
	default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
			break;
	}
}

static void
hev_filebox_uploader_get_property(GObject *obj,
			guint prop_id, GValue *value, GParamSpec *pspec)
{
	HevFileboxUploader *self = HEV_FILEBOX_UPLOADER(obj);
	HevFileboxUploaderPrivate *priv = HEV_FILEBOX_UPLOADER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	switch(prop_id) {
	case PROP_CONFIG:
			g_value_set_pointer(value, priv->config);
			break;
	default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
			break;
	}
}

static void
hev_filebox_uploader_class_init (HevFileboxUploaderClass *klass)
{
    GObjectClass *obj_class = G_OBJECT_CLASS (klass);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    obj_class->constructor = hev_filebox_uploader_constructor;
    obj_class->constructed = hev_filebox_uploader_constructed;
    obj_class->dispose = hev_filebox_uploader_dispose;
    obj_class->finalize = hev_filebox_uploader_finalize;

	obj_class->set_property = hev_filebox_uploader_set_property;
	obj_class->get_property = hev_filebox_uploader_get_property;

	hev_filebox_uploader_properties[PROP_CONFIG] =
		g_param_spec_pointer ("config", "Config", "The module config",
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_properties(obj_class, N_PROPERTIES,
				hev_filebox_uploader_properties);

    g_type_class_add_private (klass, sizeof (HevFileboxUploaderPrivate));
}

static void
hev_filebox_uploader_init (HevFileboxUploader *self)
{
    HevFileboxUploaderPrivate *priv = HEV_FILEBOX_UPLOADER_GET_PRIVATE (self);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

GObject *
hev_filebox_uploader_new (GKeyFile *config)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    return g_object_new (HEV_TYPE_FILEBOX_UPLOADER, "config", config, NULL);
}

void
hev_filebox_uploader_handle_async (HevFileboxUploader *self, GObject *scgi_task,
			GAsyncReadyCallback callback, gpointer user_data)
{
	GTask *task = NULL;

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	task = g_task_new (self, NULL, callback, user_data);
	g_task_set_task_data (task, g_object_ref (scgi_task), (GDestroyNotify) g_object_unref);
	g_task_run_in_thread (task, filebox_uploader_handle_task_handler);
	g_object_unref (task);
}

gboolean
hev_filebox_uploader_handle_finish (HevFileboxUploader *self,
			GAsyncResult *result, GError **error)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail (g_task_is_valid (result, self), NULL);

	return g_task_propagate_boolean (G_TASK (result), error);
}

static void
filebox_uploader_handle_task_handler (GTask *task, gpointer source_object,
			gpointer task_data, GCancellable *cancellable)
{
	HevFileboxUploader *self = HEV_FILEBOX_UPLOADER (source_object);
    HevFileboxUploaderPrivate *priv = HEV_FILEBOX_UPLOADER_GET_PRIVATE (self);
	GObject *scgi_task = task_data;

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_task_return_boolean (task, TRUE);
}

