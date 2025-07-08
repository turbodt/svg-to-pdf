#ifndef APP_XML_BBOX_COLLECTION_H
#define APP_XML_BBOX_COLLECTION_H


#include <geometry.h>
#include <libxml/tree.h>

typedef struct XmlBboxItem {
    xmlNode const *node;
    Box2D bbox;
} XmlBboxItem;
typedef struct XmlBboxCollection XmlBboxCollection;


XmlBboxCollection * xml_bbox_collection_make(void);
void xml_bbox_collection_destroy(XmlBboxCollection *);


unsigned int xml_bbox_collection_get_count(XmlBboxCollection const *);
XmlBboxItem const * xml_bbox_collection_getc(
    XmlBboxCollection const *,
    unsigned int
);


int xml_bbox_collection_append(XmlBboxCollection *, xmlNode const *, Box2D);
void xml_bbox_collection_sort(
    XmlBboxCollection *,
    int(*cmp)(XmlBboxItem const *, XmlBboxItem const *)
);
void xml_bbox_collection_sort_area(XmlBboxCollection *);
void xml_bbox_collection_sort_dim(XmlBboxCollection *);


#endif
