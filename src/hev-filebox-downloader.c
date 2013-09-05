/*
 ============================================================================
 Name        : hev-filebox-downloader.c
 Author      : Heiher <root@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2013 everyone.
 Description : 
 ============================================================================
 */

#include <hev-scgi-1.0.h>

#include "hev-filebox-downloader.h"

enum
{
	PROP_0,
	PROP_CONFIG,
	N_PROPERTIES
};

static GParamSpec *hev_filebox_downloader_properties[N_PROPERTIES] = { NULL };

#define HEV_FILEBOX_DOWNLOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                HEV_TYPE_FILEBOX_DOWNLOADER, HevFileboxDownloaderPrivate))

typedef struct _HevFileboxDownloaderPrivate HevFileboxDownloaderPrivate;

struct _HevFileboxDownloaderPrivate
{
	GKeyFile *config;
};

static void filebox_downloader_handle_task_handler (GTask *task, gpointer source_object,
			gpointer task_data, GCancellable *cancellable);

G_DEFINE_TYPE (HevFileboxDownloader, hev_filebox_downloader, G_TYPE_OBJECT);

static void
hev_filebox_downloader_dispose (GObject *obj)
{
    HevFileboxDownloader *self = HEV_FILEBOX_DOWNLOADER (obj);
    HevFileboxDownloaderPrivate *priv = HEV_FILEBOX_DOWNLOADER_GET_PRIVATE (self);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    G_OBJECT_CLASS (hev_filebox_downloader_parent_class)->dispose (obj);
}

static void
hev_filebox_downloader_finalize (GObject *obj)
{
    HevFileboxDownloader *self = HEV_FILEBOX_DOWNLOADER (obj);
    HevFileboxDownloaderPrivate *priv = HEV_FILEBOX_DOWNLOADER_GET_PRIVATE (self);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->config) {
		g_key_file_unref (priv->config);
		priv->config = NULL;
	}

    G_OBJECT_CLASS (hev_filebox_downloader_parent_class)->finalize (obj);
}

static GObject *
hev_filebox_downloader_constructor (GType type,
            guint n,
            GObjectConstructParam *param)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    return G_OBJECT_CLASS (hev_filebox_downloader_parent_class)->
        constructor (type, n, param);
}

static void
hev_filebox_downloader_constructed (GObject *obj)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    G_OBJECT_CLASS (hev_filebox_downloader_parent_class)->constructed (obj);
}

static void
hev_filebox_downloader_set_property(GObject *obj,
			guint prop_id, const GValue *value, GParamSpec *pspec)
{
	HevFileboxDownloader *self = HEV_FILEBOX_DOWNLOADER(obj);
	HevFileboxDownloaderPrivate *priv = HEV_FILEBOX_DOWNLOADER_GET_PRIVATE(self);

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
hev_filebox_downloader_get_property(GObject *obj,
			guint prop_id, GValue *value, GParamSpec *pspec)
{
	HevFileboxDownloader *self = HEV_FILEBOX_DOWNLOADER(obj);
	HevFileboxDownloaderPrivate *priv = HEV_FILEBOX_DOWNLOADER_GET_PRIVATE(self);

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
hev_filebox_downloader_class_init (HevFileboxDownloaderClass *klass)
{
    GObjectClass *obj_class = G_OBJECT_CLASS (klass);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    obj_class->constructor = hev_filebox_downloader_constructor;
    obj_class->constructed = hev_filebox_downloader_constructed;
    obj_class->dispose = hev_filebox_downloader_dispose;
    obj_class->finalize = hev_filebox_downloader_finalize;

	obj_class->set_property = hev_filebox_downloader_set_property;
	obj_class->get_property = hev_filebox_downloader_get_property;

	hev_filebox_downloader_properties[PROP_CONFIG] =
		g_param_spec_pointer ("config", "Config", "The module config",
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_properties(obj_class, N_PROPERTIES,
				hev_filebox_downloader_properties);

    g_type_class_add_private (klass, sizeof (HevFileboxDownloaderPrivate));
}

static void
hev_filebox_downloader_init (HevFileboxDownloader *self)
{
    HevFileboxDownloaderPrivate *priv = HEV_FILEBOX_DOWNLOADER_GET_PRIVATE (self);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

GObject *
hev_filebox_downloader_new (GKeyFile *config)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

    return g_object_new (HEV_TYPE_FILEBOX_DOWNLOADER, "config", config, NULL);
}

void
hev_filebox_downloader_handle_async (HevFileboxDownloader *self, GObject *scgi_task,
			GAsyncReadyCallback callback, gpointer user_data)
{
	GTask *task = NULL;

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	task = g_task_new (self, NULL, callback, user_data);
	g_task_set_task_data (task, g_object_ref (scgi_task), (GDestroyNotify) g_object_unref);
	g_task_run_in_thread (task, filebox_downloader_handle_task_handler);
	g_object_unref (task);
}

gboolean
hev_filebox_downloader_handle_finish (HevFileboxDownloader *self,
			GAsyncResult *result, GError **error)
{
    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail (g_task_is_valid (result, self), NULL);

	return g_task_propagate_boolean (G_TASK (result), error);
}

static void
filebox_downloader_handle_task_handler (GTask *task, gpointer source_object,
			gpointer task_data, GCancellable *cancellable)
{
	HevFileboxDownloader *self = HEV_FILEBOX_DOWNLOADER (source_object);
    HevFileboxDownloaderPrivate *priv = HEV_FILEBOX_DOWNLOADER_GET_PRIVATE (self);
	GObject *scgi_task = task_data;
	gboolean status = TRUE;
	GObject *request = NULL;
	GHashTable *req_hash_table = NULL;
	GObject *response = NULL;
	GOutputStream *output_stream = NULL;
	GHashTable *res_hash_table = NULL;
	const gchar *request_uri = NULL;
	gchar *fp_path, *str, pattern[256];
	GRegex *regex = NULL;
	GMatchInfo *match_info = NULL;

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	request = hev_scgi_task_get_request (HEV_SCGI_TASK (scgi_task));
	req_hash_table = hev_scgi_request_get_header_hash_table (HEV_SCGI_REQUEST (request));
	response = hev_scgi_task_get_response (HEV_SCGI_TASK (scgi_task));
	output_stream = hev_scgi_response_get_output_stream (HEV_SCGI_RESPONSE (response));
	res_hash_table = hev_scgi_response_get_header_hash_table (HEV_SCGI_RESPONSE (response));

	fp_path = g_key_file_get_string (priv->config, "Module", "FilePoolPath", NULL);
	str = g_key_file_get_string (priv->config, "Module", "BaseURI", NULL);
	g_snprintf (pattern, 256, "^%s(.+)$", str);
	g_free (str);

	regex = g_regex_new (pattern, 0, 0, NULL);
	request_uri = g_hash_table_lookup (req_hash_table, "REQUEST_URI");
	if (g_regex_match (regex, request_uri, 0, &match_info)) { /* down */
		const gchar *filename = g_match_info_fetch (match_info, 1);
		gchar *path = NULL;
		GFile *file = NULL;
		gboolean exists;

		path = g_build_filename (fp_path, filename, NULL);
		file = g_file_new_for_path (path);
		g_free (path);

		exists = g_file_query_exists (file, NULL);
		if (exists) {
			if (G_FILE_TYPE_REGULAR == g_file_query_file_type (file, G_FILE_QUERY_INFO_NONE, NULL)) {
				gchar *mime_type = NULL;
				GFileInfo *file_info = g_file_query_info (file,
							G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE","G_FILE_ATTRIBUTE_STANDARD_SIZE,
							G_FILE_QUERY_INFO_NONE, NULL, NULL);
				mime_type = g_content_type_get_mime_type (g_file_info_get_content_type (file_info));
				g_hash_table_insert (res_hash_table, g_strdup ("Status"), g_strdup ("200 OK"));
				g_hash_table_insert (res_hash_table, g_strdup ("Content-Type"),
							g_strdup (mime_type ? mime_type : "application/octet-stream"));
				g_hash_table_insert (res_hash_table, g_strdup ("Content-Length"),
							g_strdup_printf ("%lu", g_file_info_get_size (file_info)));
				g_object_unref (file_info);
			} else {
				g_hash_table_insert (res_hash_table, g_strdup ("Status"), g_strdup ("403 Forbidden"));
				exists = FALSE;
			}
		} else {
			g_hash_table_insert (res_hash_table, g_strdup ("Status"), g_strdup ("404 Not Found"));
		}
		hev_scgi_response_write_header (HEV_SCGI_RESPONSE (response));

		if (exists) {
			gint len;
			GFileInputStream *istream = g_file_read (file, NULL, NULL);

			do {
				gchar buffer[8192];
				gint i = 0;

				len = g_input_stream_read (G_INPUT_STREAM (istream),
							buffer, 8192, NULL, NULL);
				if (-1 == len) {
					status = FALSE;
					break;
				}

				do {
					gssize ret = g_output_stream_write (output_stream,
								buffer + i, len - i, NULL, NULL);
					if (-1 == ret) {
						status = FALSE;
						len = -1;
						break;
					}
					i += ret;
				} while (i < len);
			} while (0 < len);

			g_object_unref (istream);
		}

		g_object_unref (file);
	} else { /* list */
		GFile *fp_file = NULL;
		GFileEnumerator *enumerator = NULL;
		GFileInfo *file_info = NULL;

		g_hash_table_insert (res_hash_table, g_strdup ("Status"), g_strdup ("200 OK"));
		g_hash_table_insert (res_hash_table, g_strdup ("Content-Type"), g_strdup ("text/plain"));
		hev_scgi_response_write_header (HEV_SCGI_RESPONSE (response));

		fp_file = g_file_new_for_path (fp_path);
		enumerator = g_file_enumerate_children (fp_file,
					G_FILE_ATTRIBUTE_STANDARD_NAME","G_FILE_ATTRIBUTE_STANDARD_TYPE,
					G_FILE_QUERY_INFO_NONE, NULL, NULL);
		while (status && (file_info = g_file_enumerator_next_file (enumerator, NULL, NULL))) {
			if (G_FILE_TYPE_REGULAR == g_file_info_get_file_type (file_info)) {
				gchar buffer[1024];
				gint i = 0, len;

				len = g_snprintf (buffer, 1024, "%s\r\n", g_file_info_get_name (file_info));
				do {
					gssize ret = g_output_stream_write (output_stream,
								buffer + i, len - i, NULL, NULL);
					if (-1 == ret) {
						status = FALSE;
						break;
					}
					i += ret;
				} while (i < len);
			}
			g_object_unref (file_info);
		}
		g_object_unref (enumerator);
		g_object_unref (fp_file);
	}
	g_match_info_free (match_info);
	g_regex_unref (regex);
	g_free (fp_path);

	g_task_return_boolean (task, status);
}

