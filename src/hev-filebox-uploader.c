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
#include <string.h>

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
static GFile * filebox_uploader_handle_task_create_tmp (HevFileboxUploader *self,
			GObject *task, GInputStream *req_stream, GHashTable *req_htb,
			GHashTable *res_htb, const gchar *ft_path, gsize size);
static GPtrArray * filebox_uploader_handle_task_split_tmp (HevFileboxUploader *self,
			GObject *task, GMappedFile *mapped_file, GHashTable *res_htb,
			const gchar *fp_path, const gchar *fm_path, const gchar *boundary);
static void file_ptr_array_foreach_delete_handler (gpointer data, gpointer user_data);
static GFile * filebox_uploader_handle_task_create_file (HevFileboxUploader *self,
			GHashTable *res_htb, const gchar *fp_path, const gchar *fm_path,
			const gchar *filename, const gchar *contents, gsize length);

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
	gboolean status = TRUE;
	GObject *request = NULL;
	GInputStream *req_stream = NULL;
	GHashTable *req_htb = NULL;
	GObject *response = NULL;
	GHashTable *res_htb = NULL;
	const gchar *content_type = NULL, *content_length = NULL;
	GRegex *regex = NULL;
	GMatchInfo *match_info = NULL;
	gchar *boundary = NULL, *fp_path = NULL, *fm_path = NULL, *ft_path = NULL;
	guint64 length = 0;
	GFile *file_tmp = NULL;

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	request = hev_scgi_task_get_request (HEV_SCGI_TASK (scgi_task));
	req_stream = hev_scgi_request_get_input_stream (HEV_SCGI_REQUEST (request));
	req_htb = hev_scgi_request_get_header_hash_table (HEV_SCGI_REQUEST (request));
	response = hev_scgi_task_get_response (HEV_SCGI_TASK (scgi_task));
	res_htb = hev_scgi_response_get_header_hash_table (HEV_SCGI_RESPONSE (response));

	content_type = g_hash_table_lookup (req_htb, "CONTENT_TYPE");
	content_length = g_hash_table_lookup (req_htb, "CONTENT_LENGTH");

	/* get boundary string from content type */
	regex = g_regex_new ("^multipart/form-data;\\s*boundary=(.+)$", 0, 0, NULL);
	if (!g_regex_match (regex, content_type, 0, &match_info)) {
		g_hash_table_insert (res_htb, g_strdup ("Status"), g_strdup ("400 Bad Request"));
		hev_scgi_response_write_header (HEV_SCGI_RESPONSE (response));
		g_regex_unref (regex);
		g_task_return_boolean (task, FALSE);
		return;
	}
	boundary = g_match_info_fetch (match_info, 1);
	g_match_info_unref (match_info);
	g_regex_unref (regex);

	fp_path = g_key_file_get_string (priv->config, "Module", "FilePoolPath", NULL);
	fm_path = g_key_file_get_string (priv->config, "Module", "FileMetaPath", NULL);
	ft_path = g_key_file_get_string (priv->config, "Module", "FileTempPath", NULL);

	length = g_ascii_strtoull (content_length, NULL, 10);

	/* create tmp file */
	file_tmp = filebox_uploader_handle_task_create_tmp (self, scgi_task,
				req_stream, req_htb, res_htb, ft_path, length);
	if (file_tmp) {
		gchar *path = NULL;
		GMappedFile *mapped_file = NULL;

		path = g_file_get_path (file_tmp);
		mapped_file = g_mapped_file_new (path, FALSE, NULL);
		g_free (path);
		if (mapped_file) {
			GPtrArray *files = NULL;

			/* split files from tmp file */
			files = filebox_uploader_handle_task_split_tmp (self, scgi_task,
						mapped_file, res_htb, fp_path, fm_path, boundary);
			if (files) {
				// write meta files

				g_ptr_array_unref (files);
			}
			
			g_mapped_file_unref (mapped_file);
		}
		
		g_file_delete (file_tmp, NULL, NULL);
		g_object_unref (file_tmp);
	}

	g_free (boundary);
	g_free (fp_path);
	g_free (fm_path);
	g_free (ft_path);

	if (!g_hash_table_contains (res_htb, "Status"))
	  g_hash_table_insert (res_htb, g_strdup ("Status"), g_strdup ("200 OK"));
	hev_scgi_response_write_header (HEV_SCGI_RESPONSE (response));

	g_task_return_boolean (task, status);
}

static GFile *
filebox_uploader_handle_task_create_tmp (HevFileboxUploader *self,
			GObject *task, GInputStream *req_stream, GHashTable *req_htb,
			GHashTable *res_htb, const gchar *ft_path, gsize size)
{
	GFile *file = NULL;
	gboolean status = TRUE;
	gchar name[256], *path = NULL;
	const gchar *remote_addr = NULL, *remote_port = NULL;
	GFileOutputStream *file_ostream = NULL;

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	remote_addr = g_hash_table_lookup (req_htb, "REMOTE_ADDR");
	remote_port = g_hash_table_lookup (req_htb, "REMOTE_PORT");

	g_snprintf (name, 256, "%s-%s.tmp", remote_addr, remote_port);
	path = g_build_filename (ft_path, name, NULL);
	file = g_file_new_for_path (path);
	g_free (path);

	file_ostream = g_file_create (file, G_FILE_CREATE_PRIVATE, NULL, NULL);
	if (!file_ostream) {
		g_hash_table_insert (res_htb, g_strdup ("Status"), g_strdup ("403 Forbidden"));
		g_object_unref (file);
		return NULL;
	}

	/* write to tmp file */
	for (; 0<size;) {
		guint8 buffer[8192];
		gssize rsize = 0;

		rsize = g_input_stream_read (req_stream,
					buffer, 8192, NULL, NULL);
		if (-1 == rsize) {
			status = FALSE;
			break;
		} else if (0 == rsize) {
			break;
		}

		if (!g_output_stream_write_all (G_OUTPUT_STREAM (file_ostream),
						buffer, rsize, NULL, NULL, NULL)) {
			status = FALSE;
			break;
		}
		size -= rsize;
	}

	g_object_unref (file_ostream);

	if (!status) {
		g_hash_table_insert (res_htb, g_strdup ("Status"),
					g_strdup ("500 Internal Server Error"));
		g_object_unref (file);
		return NULL;
	}

	return file;
}

static GPtrArray *
filebox_uploader_handle_task_split_tmp (HevFileboxUploader *self,
			GObject *task, GMappedFile *mapped_file, GHashTable *res_htb,
			const gchar *fp_path, const gchar *fm_path, const gchar *boundary)
{
	GPtrArray *files = NULL;
	gchar *contents = NULL, *pi = NULL, *ps = NULL, *pe = NULL;
	gsize i = 0, length = 0, blen = strlen (boundary);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	contents = g_mapped_file_get_contents (mapped_file);
	length = g_mapped_file_get_length (mapped_file);

	files = g_ptr_array_new_full (5, (GDestroyNotify) g_object_unref);
	if (!files) {
		g_hash_table_insert (res_htb, g_strdup ("Status"),
					g_strdup ("500 Internal Server Error"));
		return NULL;
	}

	/* split tmp file */
	for (i=0; i<length; i++) {
		gchar *p = NULL;

		/* lookup boundary start tag */
		if (('-' != contents[i]) ||
			(i >= length) || ('-' != contents[i + 1]))
		  continue;
		p = contents + i + 2;

		/* check boundary */
		if (0 == memcmp (p, boundary, blen)) {
			if (!pi)
			  pi = p + blen;
			else if (!pe)
			  pe = p - 4;
			i += blen;
		}

		/* found a part */
		if (pi && pe) {
			gchar *filename = NULL;
			/* get part data start address */
			ps = g_strstr_len (pi, length - (pi - contents),
						"\r\n\r\n");
			if (!ps) continue;
			ps += 4;

			/* check part type (file or attributes) */
			filename = g_strstr_len (pi, ps - pi, "filename=");
			if (filename) { /* is a file */
				GFile *file = NULL;

				file = filebox_uploader_handle_task_create_file (self,
							res_htb, fp_path, fm_path, filename,
							ps, pe - ps);
				if (!file) {
					g_ptr_array_foreach (files,
								file_ptr_array_foreach_delete_handler,
								NULL);
					g_ptr_array_unref (files);
					files = NULL;
					break;
				}
				g_ptr_array_add (files, file);
			} else { /* other attributes */
				// attribute
			}

			/* find next part init */
			pi = pe + 4 + blen;
			ps = NULL;
			pe = NULL;
		}
	}

	return files;
}

static void
file_ptr_array_foreach_delete_handler (gpointer data, gpointer user_data)
{
	GFile *file = G_FILE (data);

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_file_delete (file, NULL, NULL);
}

static GFile *
filebox_uploader_handle_task_create_file (HevFileboxUploader *self,
			GHashTable *res_htb, const gchar *fp_path, const gchar *fm_path,
			const gchar *filename, const gchar *contents, gsize length)
{
	GFile *file = NULL;
	GRegex *regex = NULL;
	GMatchInfo *match_info = NULL;
	gchar *file_name = NULL, *file_path = NULL, *meta_path = NULL;
	GFileOutputStream *file_ostream = NULL;

    g_debug ("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	/* fetch filename */
	regex = g_regex_new ("filename=\"(.+)\"", 0, 0, NULL);
	if (g_regex_match (regex, filename, 0, &match_info))
	  file_name = g_match_info_fetch (match_info, 1);
	g_match_info_unref (match_info);
	g_regex_unref (regex);

	/* build file path and meta path */
	file_path = g_build_filename (fp_path, file_name, NULL);
	meta_path = g_build_filename (fm_path, file_name, NULL);
	g_free (file_name);

	/* new file */
	file = g_file_new_for_path (file_path);
	g_free (file_path);
	if (!file) {
		g_hash_table_insert (res_htb, g_strdup ("Status"),
					g_strdup ("500 Internal Server Error"));
		g_free (meta_path);
		return NULL;
	}

	/* craete file */
	file_ostream = g_file_create (file, G_FILE_CREATE_PRIVATE, NULL, NULL);
	if (!file_ostream) {
		g_hash_table_insert (res_htb, g_strdup ("Status"),
					g_strdup ("403 Forbidden"));
		g_object_unref (file);
		g_free (meta_path);
		return NULL;
	}

	/* write contents to file */
	if (!g_output_stream_write_all (G_OUTPUT_STREAM (file_ostream),
				contents, length, NULL, NULL, NULL)) {
		g_hash_table_insert (res_htb, g_strdup ("Status"),
					g_strdup ("500 Internal Server Error"));
		g_object_unref (file_ostream);
		g_object_unref (file);
		g_free (meta_path);
		return NULL;
	}

	g_object_unref (file_ostream);
	g_free (meta_path);

	return file;
}

