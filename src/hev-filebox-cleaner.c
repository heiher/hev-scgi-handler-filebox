/*
 ============================================================================
 Name        : hev-filebox-cleaner.c
 Author      : Heiher <root@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2013 everyone.
 Description : 
 ============================================================================
 */

#include <hev-scgi-1.0.h>
#include <string.h>

#include "hev-filebox-cleaner.h"

enum
{
	PROP_0,
	PROP_CONFIG,
	N_PROPERTIES
};

static GParamSpec *hev_filebox_cleaner_properties[N_PROPERTIES] = { NULL };

#define HEV_FILEBOX_CLEANER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                HEV_TYPE_FILEBOX_CLEANER, HevFileboxCleanerPrivate))

typedef struct _HevFileboxCleanerPrivate HevFileboxCleanerPrivate;

struct _HevFileboxCleanerPrivate
{
	GKeyFile *config;

	guint timeout_tag;
};

static gboolean cleaner_timeout_handler (gpointer user_data);
static void cleaner_task_ready_handler (GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void cleaner_task_handler (GTask *task, gpointer source_object,
			gpointer task_data, GCancellable *cancellable);

G_DEFINE_TYPE (HevFileboxCleaner, hev_filebox_cleaner, G_TYPE_OBJECT);

static void
hev_filebox_cleaner_dispose (GObject *obj)
{
    HevFileboxCleaner *self = HEV_FILEBOX_CLEANER (obj);
    HevFileboxCleanerPrivate *priv = HEV_FILEBOX_CLEANER_GET_PRIVATE (self);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    G_OBJECT_CLASS (hev_filebox_cleaner_parent_class)->dispose (obj);
}

static void
hev_filebox_cleaner_finalize (GObject *obj)
{
    HevFileboxCleaner *self = HEV_FILEBOX_CLEANER (obj);
    HevFileboxCleanerPrivate *priv = HEV_FILEBOX_CLEANER_GET_PRIVATE (self);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if (0 < priv->timeout_tag) {
		g_source_remove (priv->timeout_tag);
		priv->timeout_tag = 0;
	}

	if(priv->config) {
		g_key_file_unref (priv->config);
		priv->config = NULL;
	}

    G_OBJECT_CLASS (hev_filebox_cleaner_parent_class)->finalize (obj);
}

static GObject *
hev_filebox_cleaner_constructor (GType type,
            guint n,
            GObjectConstructParam *param)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    return G_OBJECT_CLASS (hev_filebox_cleaner_parent_class)->
        constructor (type, n, param);
}

static void
hev_filebox_cleaner_constructed (GObject *obj)
{
	HevFileboxCleaner *self = HEV_FILEBOX_CLEANER(obj);
	HevFileboxCleanerPrivate *priv = HEV_FILEBOX_CLEANER_GET_PRIVATE(self);
	gint interval = 0;

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	interval = g_key_file_get_integer (priv->config,
				"Module", "CleanInterval", NULL);
	priv->timeout_tag = g_timeout_add_seconds (interval,
				cleaner_timeout_handler, self);

    G_OBJECT_CLASS (hev_filebox_cleaner_parent_class)->constructed (obj);
}

static void
hev_filebox_cleaner_set_property(GObject *obj,
			guint prop_id, const GValue *value, GParamSpec *pspec)
{
	HevFileboxCleaner *self = HEV_FILEBOX_CLEANER(obj);
	HevFileboxCleanerPrivate *priv = HEV_FILEBOX_CLEANER_GET_PRIVATE(self);

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
hev_filebox_cleaner_get_property(GObject *obj,
			guint prop_id, GValue *value, GParamSpec *pspec)
{
	HevFileboxCleaner *self = HEV_FILEBOX_CLEANER(obj);
	HevFileboxCleanerPrivate *priv = HEV_FILEBOX_CLEANER_GET_PRIVATE(self);

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
hev_filebox_cleaner_class_init (HevFileboxCleanerClass *klass)
{
    GObjectClass *obj_class = G_OBJECT_CLASS (klass);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    obj_class->constructor = hev_filebox_cleaner_constructor;
    obj_class->constructed = hev_filebox_cleaner_constructed;
    obj_class->dispose = hev_filebox_cleaner_dispose;
    obj_class->finalize = hev_filebox_cleaner_finalize;

	obj_class->set_property = hev_filebox_cleaner_set_property;
	obj_class->get_property = hev_filebox_cleaner_get_property;

	hev_filebox_cleaner_properties[PROP_CONFIG] =
		g_param_spec_pointer ("config", "Config", "The module config",
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_properties(obj_class, N_PROPERTIES,
				hev_filebox_cleaner_properties);

    g_type_class_add_private (klass, sizeof (HevFileboxCleanerPrivate));
}

static void
hev_filebox_cleaner_init (HevFileboxCleaner *self)
{
    HevFileboxCleanerPrivate *priv = HEV_FILEBOX_CLEANER_GET_PRIVATE (self);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

GObject *
hev_filebox_cleaner_new (GKeyFile *config)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    return g_object_new (HEV_TYPE_FILEBOX_CLEANER, "config", config, NULL);
}

static gboolean
cleaner_timeout_handler (gpointer user_data)
{
	HevFileboxCleaner *self = HEV_FILEBOX_CLEANER (user_data);
	GTask *task = NULL;

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	task = g_task_new (self, NULL, cleaner_task_ready_handler, NULL);
	g_task_run_in_thread (task, cleaner_task_handler);
	g_object_unref (task);

	return G_SOURCE_CONTINUE;
}

static void
cleaner_task_ready_handler (GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail (g_task_is_valid (res, source_object), NULL);
	g_task_propagate_boolean (G_TASK (res), NULL);
}

static void
cleaner_task_handler (GTask *task, gpointer source_object,
			gpointer task_data, GCancellable *cancellable)
{
	HevFileboxCleaner *self = HEV_FILEBOX_CLEANER (source_object);
    HevFileboxCleanerPrivate *priv = HEV_FILEBOX_CLEANER_GET_PRIVATE (self);
	gchar *fp_path = NULL, *fm_path = NULL;
	GFile *file = NULL;
	GFileEnumerator *file_enumerator = NULL;
	GFileInfo *file_info = NULL;

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	fp_path = g_key_file_get_string (priv->config, "Module", "FilePoolPath", NULL);
	fm_path = g_key_file_get_string (priv->config, "Module", "FileMetaPath", NULL);

	/* enumerate file pool */
	file = g_file_new_for_path (fp_path);
	file_enumerator = g_file_enumerate_children (file,
				G_FILE_ATTRIBUTE_STANDARD_NAME","G_FILE_ATTRIBUTE_STANDARD_TYPE,
				G_FILE_QUERY_INFO_NONE, NULL, NULL);
	while (file_info = g_file_enumerator_next_file (file_enumerator, NULL, NULL)) {
		if (G_FILE_TYPE_REGULAR == g_file_info_get_file_type (file_info)) {
			gchar *meta_path = NULL;
			GKeyFile *meta = NULL;

			meta_path = g_build_filename (fm_path, g_file_info_get_name (file_info), NULL);
			meta = g_key_file_new ();
			if (g_key_file_load_from_file (meta, meta_path, G_KEY_FILE_NONE, NULL)) {
				gint64 exp = 0;
				GDateTime *now_date = NULL, *exp_date = NULL;

				exp = g_key_file_get_int64 (meta, "Meta", "ExpDate", NULL);
				now_date = g_date_time_new_now_utc ();
				exp_date = g_date_time_new_from_unix_utc (exp);

				/* compare date */
				if (1 == g_date_time_compare (now_date, exp_date)) {
					gchar *file_path = NULL;

					file_path = g_build_filename (fp_path,
								g_file_info_get_name (file_info), NULL);
					/* delete file and it's meta */
					g_unlink ((const gchar *) file_path);
					g_unlink ((const gchar *) meta_path);
					g_free (file_path);
				}

				g_date_time_unref (now_date);
				g_date_time_unref (exp_date);
			}
			g_key_file_unref (meta);
			g_free (meta_path);
		}
		g_object_unref (file_info);
	}
	g_object_unref (file_enumerator);
	g_object_unref (file);

	g_free (fp_path);
	g_free (fm_path);

	g_task_return_boolean (task, TRUE);
}

