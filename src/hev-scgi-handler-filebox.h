/*
 ============================================================================
 Name        : hev-scgi-handler-filebox.h
 Author      : Heiher <root@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2013 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_SCGI_HANDLER_FILEBOX_H__
#define __HEV_SCGI_HANDLER_FILEBOX_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HEV_TYPE_SCGI_HANDLER_FILEBOX	(hev_scgi_handler_filebox_get_type())
#define HEV_SCGI_HANDLER_FILEBOX(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_SCGI_HANDLER_FILEBOX, HevSCGIHandlerFilebox))
#define HEV_IS_SCGI_HANDLER_FILEBOX(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_SCGI_HANDLER_FILEBOX))
#define HEV_SCGI_HANDLER_FILEBOX_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_SCGI_HANDLER_FILEBOX, HevSCGIHandlerFileboxClass))
#define HEV_IS_SCGI_HANDLER_FILEBOX_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_SCGI_HANDLER_FILEBOX))
#define HEV_SCGI_HANDLER_FILEBOX_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_SCGI_HANDLER_FILEBOX, HevSCGIHandlerFileboxClass))

#ifdef STATIC_MODULE
#define HEV_REGISTER_TYPE_SCGI_HANDLER_FILEBOX() (HEV_TYPE_SCGI_HANDLER_FILEBOX)
#else /* STATIC_MODULE */
#define HEV_REGISTER_TYPE_SCGI_HANDLER_FILEBOX(module) (hev_scgi_handler_filebox_reg_type(module))
#endif /* !STATIC_MODULE */

typedef struct _HevSCGIHandlerFilebox HevSCGIHandlerFilebox;
typedef struct _HevSCGIHandlerFileboxClass HevSCGIHandlerFileboxClass;

struct _HevSCGIHandlerFilebox
{
	GObject parent_instance;
};

struct _HevSCGIHandlerFileboxClass
{
	GObjectClass parent_class;
};

#ifndef STATIC_MODULE
void hev_scgi_handler_filebox_reg_type(GTypeModule *module);
#endif /* !STATIC_MODULE */

GType hev_scgi_handler_filebox_get_type(void);

G_END_DECLS

#endif /* __HEV_SCGI_HANDLER_FILEBOX_H__ */

