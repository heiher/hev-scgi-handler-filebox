/*
 ============================================================================
 Name        : hev-filebox-deleter.c
 Author      : Heiher <root@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2013 everyone.
 Description :
 ============================================================================
 */

#include <string.h>
#include <hev-scgi-1.0.h>

#include "hev-filebox-deleter.h"

#define SUPER_PASSWORD	"password"

enum
{
	PROP_0,
	PROP_CONFIG,
	N_PROPERTIES
};

static GParamSpec *hev_filebox_deleter_properties[N_PROPERTIES] = { NULL };

#define HEV_FILEBOX_DELETER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                HEV_TYPE_FILEBOX_DELETER, HevFileboxDeleterPrivate))

typedef struct _HevFileboxDeleterPrivate HevFileboxDeleterPrivate;

struct _HevFileboxDeleterPrivate
{
	GKeyFile *config;
};

static void filebox_deleter_handle_task_handler (GTask *task, gpointer source_object,
			gpointer task_data, GCancellable *cancellable);
static gboolean filebox_deleter_handle_task_delete (const gchar *fp_path, const gchar *fm_path);

G_DEFINE_TYPE (HevFileboxDeleter, hev_filebox_deleter, G_TYPE_OBJECT);

static void
hev_filebox_deleter_dispose (GObject *obj)
{
	HevFileboxDeleter *self = HEV_FILEBOX_DELETER (obj);
	HevFileboxDeleterPrivate *priv = HEV_FILEBOX_DELETER_GET_PRIVATE (self);

	g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS (hev_filebox_deleter_parent_class)->dispose (obj);
}

static void
hev_filebox_deleter_finalize (GObject *obj)
{
	HevFileboxDeleter *self = HEV_FILEBOX_DELETER (obj);
	HevFileboxDeleterPrivate *priv = HEV_FILEBOX_DELETER_GET_PRIVATE (self);

	g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->config) {
		g_key_file_unref (priv->config);
		priv->config = NULL;
	}

	G_OBJECT_CLASS (hev_filebox_deleter_parent_class)->finalize (obj);
}

static GObject *
hev_filebox_deleter_constructor (GType type,
            guint n,
            GObjectConstructParam *param)
{
	g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	return G_OBJECT_CLASS (hev_filebox_deleter_parent_class)->constructor (type, n, param);
}

static void
hev_filebox_deleter_constructed (GObject *obj)
{
	g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS (hev_filebox_deleter_parent_class)->constructed (obj);
}

static void
hev_filebox_deleter_set_property(GObject *obj,
			guint prop_id, const GValue *value, GParamSpec *pspec)
{
	HevFileboxDeleter *self = HEV_FILEBOX_DELETER(obj);
	HevFileboxDeleterPrivate *priv = HEV_FILEBOX_DELETER_GET_PRIVATE(self);

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
hev_filebox_deleter_get_property(GObject *obj,
			guint prop_id, GValue *value, GParamSpec *pspec)
{
	HevFileboxDeleter *self = HEV_FILEBOX_DELETER(obj);
	HevFileboxDeleterPrivate *priv = HEV_FILEBOX_DELETER_GET_PRIVATE(self);

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
hev_filebox_deleter_class_init (HevFileboxDeleterClass *klass)
{
	GObjectClass *obj_class = G_OBJECT_CLASS (klass);

	g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_filebox_deleter_constructor;
	obj_class->constructed = hev_filebox_deleter_constructed;
	obj_class->dispose = hev_filebox_deleter_dispose;
	obj_class->finalize = hev_filebox_deleter_finalize;

	obj_class->set_property = hev_filebox_deleter_set_property;
	obj_class->get_property = hev_filebox_deleter_get_property;

	hev_filebox_deleter_properties[PROP_CONFIG] =
		g_param_spec_pointer ("config", "Config", "The module config",
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_properties(obj_class, N_PROPERTIES,
				hev_filebox_deleter_properties);

	g_type_class_add_private (klass, sizeof (HevFileboxDeleterPrivate));
}

static void
hev_filebox_deleter_init (HevFileboxDeleter *self)
{
	HevFileboxDeleterPrivate *priv = HEV_FILEBOX_DELETER_GET_PRIVATE (self);

	g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

GObject *
hev_filebox_deleter_new (GKeyFile *config)
{
	g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	return g_object_new (HEV_TYPE_FILEBOX_DELETER, "config", config, NULL);
}

void
hev_filebox_deleter_handle_async (HevFileboxDeleter *self, GObject *scgi_task,
			GAsyncReadyCallback callback, gpointer user_data)
{
	GTask *task = NULL;

	g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	task = g_task_new (self, NULL, callback, user_data);
	g_task_set_task_data (task, g_object_ref (scgi_task), (GDestroyNotify) g_object_unref);
	g_task_run_in_thread (task, filebox_deleter_handle_task_handler);
	g_object_unref (task);
}

gboolean
hev_filebox_deleter_handle_finish (HevFileboxDeleter *self,
			GAsyncResult *result, GError **error)
{
	g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail (g_task_is_valid (result, self), NULL);

	return g_task_propagate_boolean (G_TASK (result), error);
}

static void
filebox_deleter_handle_task_handler (GTask *task, gpointer source_object,
			gpointer task_data, GCancellable *cancellable)
{
	HevFileboxDeleter *self = HEV_FILEBOX_DELETER (source_object);
	HevFileboxDeleterPrivate *priv = HEV_FILEBOX_DELETER_GET_PRIVATE (self);
	GObject *scgi_task = task_data;
	gboolean status = TRUE;
	GObject *request = NULL;
	GHashTable *req_htb = NULL;
	GObject *response = NULL;
	GHashTable *res_htb = NULL;
	GOutputStream *res_stream = NULL;
	gchar *fp_path, *fm_path, *base_uri, pattern[256];
	GRegex *regex = NULL;
	GMatchInfo *match_info = NULL;
	const gchar *request_uri = NULL;

	g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	request = hev_scgi_task_get_request (HEV_SCGI_TASK (scgi_task));
	req_htb = hev_scgi_request_get_header_hash_table (HEV_SCGI_REQUEST (request));
	response = hev_scgi_task_get_response (HEV_SCGI_TASK (scgi_task));
	res_htb = hev_scgi_response_get_header_hash_table (HEV_SCGI_RESPONSE (response));
	res_stream = hev_scgi_response_get_output_stream (HEV_SCGI_RESPONSE (response));

	fp_path = g_key_file_get_string (priv->config, "Module", "FilePoolPath", NULL);
	fm_path = g_key_file_get_string (priv->config, "Module", "FileMetaPath", NULL);
	base_uri = g_key_file_get_string (priv->config, "Module", "BaseURI", NULL);
	g_snprintf (pattern, 256, "^%sdelete\\?file=(.+)&pass=(.+)$", base_uri);
	g_free (base_uri);

	regex = g_regex_new (pattern, 0, 0, NULL);
	request_uri = g_hash_table_lookup (req_htb, "REQUEST_URI");

	if (g_regex_match (regex, request_uri, 0, &match_info)) {
		gchar *filename, *basename, *pool_path, *meta_path, *mpass = NULL;
		gchar *file_name = g_match_info_fetch (match_info, 1);
		gchar *pass = g_match_info_fetch (match_info, 2);
		GKeyFile *key_file;

		filename = g_uri_unescape_string (file_name, "");
		basename = g_path_get_basename (filename);
		pool_path = g_build_filename (fp_path, filename, NULL);
		meta_path = g_build_filename (fm_path, filename, NULL);

		key_file = g_key_file_new ();
		if (g_key_file_load_from_file (key_file, meta_path, G_KEY_FILE_NONE, NULL))
		      mpass = g_key_file_get_string (key_file, "Meta", "RandPass", NULL);
		g_key_file_unref (key_file);

		/* check pass */
		if ((mpass && 0 == strcmp (mpass, pass)) || (0 == strcmp (SUPER_PASSWORD, pass))) {
			/* unlink */
			if (filebox_deleter_handle_task_delete (pool_path, meta_path))
			      g_hash_table_insert (res_htb, g_strdup ("Status"), g_strdup ("200 OK"));
			else
			      g_hash_table_insert (res_htb, g_strdup ("Status"), g_strdup ("406 Not Acceptable"));
		} else {
			g_hash_table_insert (res_htb, g_strdup ("Status"), g_strdup ("403 Forbidden"));
		}
		hev_scgi_response_write_header (HEV_SCGI_RESPONSE (response), NULL);

		g_free (pool_path);
		g_free (meta_path);
		g_free (filename);
		g_free (basename);
		g_free (pass);
		g_free (mpass);
		g_free (file_name);
	} else {
		g_hash_table_insert (res_htb, g_strdup ("Status"), g_strdup ("400 Bad Request"));
		hev_scgi_response_write_header (HEV_SCGI_RESPONSE (response), NULL);
	}

	g_match_info_unref (match_info);
	g_regex_unref (regex);
	g_free (fp_path);
	g_free (fm_path);

	g_task_return_boolean (task, status);
}

static gboolean
filebox_deleter_handle_task_delete (const gchar *fp_path, const gchar *fm_path)
{
	gint pool_ret, meta_ret;

	pool_ret = g_unlink (fp_path);
	meta_ret = g_unlink (fm_path);

	return 0 == (pool_ret | meta_ret);
}

