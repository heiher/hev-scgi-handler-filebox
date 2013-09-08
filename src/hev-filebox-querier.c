/*
 ============================================================================
 Name        : hev-filebox-querier.c
 Author      : Heiher <root@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2013 everyone.
 Description : 
 ============================================================================
 */

#include <hev-scgi-1.0.h>
#include <string.h>

#include "hev-filebox-querier.h"

enum
{
	PROP_0,
	PROP_CONFIG,
	N_PROPERTIES
};

static GParamSpec *hev_filebox_querier_properties[N_PROPERTIES] = { NULL };

#define HEV_FILEBOX_QUERIER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                HEV_TYPE_FILEBOX_QUERIER, HevFileboxQuerierPrivate))

typedef struct _HevFileboxQuerierPrivate HevFileboxQuerierPrivate;

struct _HevFileboxQuerierPrivate
{
	GKeyFile *config;
};

static void filebox_querier_handle_task_handler (GTask *task, gpointer source_object,
			gpointer task_data, GCancellable *cancellable);
static gchar * filebox_querier_handle_task_query_size (HevFileboxQuerier *self,
			GHashTable *res_htb, const gchar *fp_path, const gchar *filename);
static gchar * filebox_querier_handle_task_query_meta (HevFileboxQuerier *self,
			GHashTable *res_htb, const gchar *fm_path, const gchar *filename);

G_DEFINE_TYPE (HevFileboxQuerier, hev_filebox_querier, G_TYPE_OBJECT);

static void
hev_filebox_querier_dispose (GObject *obj)
{
    HevFileboxQuerier *self = HEV_FILEBOX_QUERIER (obj);
    HevFileboxQuerierPrivate *priv = HEV_FILEBOX_QUERIER_GET_PRIVATE (self);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    G_OBJECT_CLASS (hev_filebox_querier_parent_class)->dispose (obj);
}

static void
hev_filebox_querier_finalize (GObject *obj)
{
    HevFileboxQuerier *self = HEV_FILEBOX_QUERIER (obj);
    HevFileboxQuerierPrivate *priv = HEV_FILEBOX_QUERIER_GET_PRIVATE (self);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->config) {
		g_key_file_unref (priv->config);
		priv->config = NULL;
	}

    G_OBJECT_CLASS (hev_filebox_querier_parent_class)->finalize (obj);
}

static GObject *
hev_filebox_querier_constructor (GType type,
            guint n,
            GObjectConstructParam *param)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    return G_OBJECT_CLASS (hev_filebox_querier_parent_class)->
        constructor (type, n, param);
}

static void
hev_filebox_querier_constructed (GObject *obj)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    G_OBJECT_CLASS (hev_filebox_querier_parent_class)->constructed (obj);
}

static void
hev_filebox_querier_set_property(GObject *obj,
			guint prop_id, const GValue *value, GParamSpec *pspec)
{
	HevFileboxQuerier *self = HEV_FILEBOX_QUERIER(obj);
	HevFileboxQuerierPrivate *priv = HEV_FILEBOX_QUERIER_GET_PRIVATE(self);

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
hev_filebox_querier_get_property(GObject *obj,
			guint prop_id, GValue *value, GParamSpec *pspec)
{
	HevFileboxQuerier *self = HEV_FILEBOX_QUERIER(obj);
	HevFileboxQuerierPrivate *priv = HEV_FILEBOX_QUERIER_GET_PRIVATE(self);

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
hev_filebox_querier_class_init (HevFileboxQuerierClass *klass)
{
    GObjectClass *obj_class = G_OBJECT_CLASS (klass);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    obj_class->constructor = hev_filebox_querier_constructor;
    obj_class->constructed = hev_filebox_querier_constructed;
    obj_class->dispose = hev_filebox_querier_dispose;
    obj_class->finalize = hev_filebox_querier_finalize;

	obj_class->set_property = hev_filebox_querier_set_property;
	obj_class->get_property = hev_filebox_querier_get_property;

	hev_filebox_querier_properties[PROP_CONFIG] =
		g_param_spec_pointer ("config", "Config", "The module config",
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_properties(obj_class, N_PROPERTIES,
				hev_filebox_querier_properties);

    g_type_class_add_private (klass, sizeof (HevFileboxQuerierPrivate));
}

static void
hev_filebox_querier_init (HevFileboxQuerier *self)
{
    HevFileboxQuerierPrivate *priv = HEV_FILEBOX_QUERIER_GET_PRIVATE (self);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

GObject *
hev_filebox_querier_new (GKeyFile *config)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    return g_object_new (HEV_TYPE_FILEBOX_QUERIER, "config", config, NULL);
}

void
hev_filebox_querier_handle_async (HevFileboxQuerier *self, GObject *scgi_task,
			GAsyncReadyCallback callback, gpointer user_data)
{
	GTask *task = NULL;

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	task = g_task_new (self, NULL, callback, user_data);
	g_task_set_task_data (task, g_object_ref (scgi_task), (GDestroyNotify) g_object_unref);
	g_task_run_in_thread (task, filebox_querier_handle_task_handler);
	g_object_unref (task);
}

gboolean
hev_filebox_querier_handle_finish (HevFileboxQuerier *self,
			GAsyncResult *result, GError **error)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail (g_task_is_valid (result, self), NULL);

	return g_task_propagate_boolean (G_TASK (result), error);
}

static void
filebox_querier_handle_task_handler (GTask *task, gpointer source_object,
			gpointer task_data, GCancellable *cancellable)
{
	HevFileboxQuerier *self = HEV_FILEBOX_QUERIER (source_object);
    HevFileboxQuerierPrivate *priv = HEV_FILEBOX_QUERIER_GET_PRIVATE (self);
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
	g_snprintf (pattern, 256, "^%squery\\?file=(.+)$", base_uri);
	g_free (base_uri);

	regex = g_regex_new (pattern, 0, 0, NULL);
	request_uri = g_hash_table_lookup (req_htb, "REQUEST_URI");

	if (g_regex_match (regex, request_uri, 0, &match_info)) {
		gchar *file_name = g_match_info_fetch (match_info, 1);
		gchar *filename = NULL, *size = NULL, *meta = NULL;

		filename = g_uri_unescape_string (file_name, "");
		/* query file size */
		size = filebox_querier_handle_task_query_size (self, res_htb, fp_path, filename);
		/* query file meta */
		meta = filebox_querier_handle_task_query_meta (self, res_htb, fm_path, filename);
		/* write attributes */
		if (size || meta) {
			g_hash_table_insert (res_htb, g_strdup ("Status"), g_strdup ("200 OK"));
			hev_scgi_response_write_header (HEV_SCGI_RESPONSE (response));
			g_output_stream_write_all (res_stream, "File: ", 6, NULL, NULL, NULL);
			g_output_stream_write_all (res_stream, filename, strlen (filename), NULL, NULL, NULL);
			g_output_stream_write_all (res_stream, "\r\n", 2, NULL, NULL, NULL);
			if (size)
			  g_output_stream_write_all (res_stream, size, strlen (size), NULL, NULL, NULL);
			if (meta)
			  g_output_stream_write_all (res_stream, meta, strlen (meta), NULL, NULL, NULL);
		} else {
			hev_scgi_response_write_header (HEV_SCGI_RESPONSE (response));
		}

		g_free (file_name);
		g_free (filename);
		g_free (size);
		g_free (meta);
	} else {
		g_hash_table_insert (res_htb, g_strdup ("Status"), g_strdup ("400 Bad Request"));
		hev_scgi_response_write_header (HEV_SCGI_RESPONSE (response));
	}

	g_match_info_unref (match_info);
	g_regex_unref (regex);
	g_free (fp_path);
	g_free (fm_path);

	g_task_return_boolean (task, status);
}

static gchar *
filebox_querier_handle_task_query_size (HevFileboxQuerier *self,
			GHashTable *res_htb, const gchar *fp_path, const gchar *filename)
{
	gchar *size = NULL, *path = NULL;
	GFile *file = NULL;
	GFileInfo *file_info = NULL;

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	path = g_build_filename (fp_path, filename, NULL);
	file = g_file_new_for_path (path);
	g_free (path);

	file_info = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_SIZE,
				G_FILE_QUERY_INFO_NONE, NULL, NULL);
	if (!file_info)
	  g_hash_table_insert (res_htb, g_strdup ("Status"),
				  g_strdup ("404 Not Found"));
	else {
		size = g_strdup_printf ("Size: %"G_GINT64_FORMAT"\r\n",
					g_file_info_get_size (file_info));
		g_object_unref (file_info);
	}

	g_object_unref (file);

	return size;
}

static gchar *
filebox_querier_handle_task_query_meta (HevFileboxQuerier *self,
			GHashTable *res_htb, const gchar *fm_path, const gchar *filename)
{
	gchar *meta = NULL, *path = NULL;
	GKeyFile *key_file = NULL;

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	key_file = g_key_file_new ();
	path = g_build_filename (fm_path, filename, NULL);

	if (!g_key_file_load_from_file (key_file, path, G_KEY_FILE_NONE, NULL)) {
		g_hash_table_insert (res_htb, g_strdup ("Status"),
					g_strdup ("500 Internal Server Error"));
	} else {
		gchar *crt = NULL, *exp = NULL, *one_off = NULL, *ip = NULL;
		GDateTime *crt_date = NULL, *exp_date = NULL;

		/* CrtDate */
		crt_date = g_date_time_new_from_unix_utc (
					g_key_file_get_int64 (key_file, "Meta", "CrtDate", NULL));
		crt = g_date_time_format (crt_date, "%Y-%m-%d %H:%M:%S");
		g_date_time_unref (crt_date);
		/* ExpDate */
		exp_date = g_date_time_new_from_unix_utc (
					g_key_file_get_int64 (key_file, "Meta", "ExpDate", NULL));
		exp = g_date_time_format (exp_date, "%Y-%m-%d %H:%M:%S");
		g_date_time_unref (exp_date);
		/* OneOff */
		one_off = g_key_file_get_string (key_file, "Meta", "OneOff", NULL);
		/* IP */
		ip = g_key_file_get_string (key_file, "Meta", "IP", NULL);

		meta = g_strdup_printf ("CrtDate: %s\r\nExpDate: %s\r\n"
					"OneOff: %s\r\nUploaderIP: %s\r\n", crt, exp, one_off, ip);

		g_free (crt);
		g_free (exp);
		g_free (one_off);
		g_free (ip);
	}

	g_free (path);
	g_key_file_unref (key_file);

	return meta;
}

