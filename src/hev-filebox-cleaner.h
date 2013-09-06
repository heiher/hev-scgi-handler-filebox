/*
 ============================================================================
 Name        : hev-filebox-cleaner.h
 Author      : Heiher <root@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2013 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_FILEBOX_CLEANER_H__
#define __HEV_FILEBOX_CLEANER_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define HEV_TYPE_FILEBOX_CLEANER (hev_filebox_cleaner_get_type ())
#define HEV_FILEBOX_CLEANER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                HEV_TYPE_FILEBOX_CLEANER, HevFileboxCleaner))
#define HEV_IS_FILEBOX_CLEANER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                HEV_TYPE_FILEBOX_CLEANER))
#define HEV_FILEBOX_CLEANER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), \
                HEV_TYPE_FILEBOX_CLEANER, HevFileboxCleanerClass))
#define HEV_IS_FILEBOX_CLEANER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                HEV_TYPE_FILEBOX_CLEANER))
#define HEV_FILEBOX_CLEANER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                HEV_TYPE_FILEBOX_CLEANER, HevFileboxCleanerClass))

typedef struct _HevFileboxCleaner HevFileboxCleaner;
typedef struct _HevFileboxCleanerClass HevFileboxCleanerClass;

struct _HevFileboxCleaner
{
    GObject parent_instance;
};

struct _HevFileboxCleanerClass
{
    GObjectClass parent_class;
};

GType hev_filebox_cleaner_get_type (void);

GObject * hev_filebox_cleaner_new (GKeyFile *config);

G_END_DECLS

#endif /* __HEV_FILEBOX_CLEANER_H__ */

