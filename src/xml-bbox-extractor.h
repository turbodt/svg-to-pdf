#ifndef APP_XML_BBOX_EXTRACTOR_H
#define APP_XML_BBOX_EXTRACTOR_H


#include "./xml-bbox-collection.h"
#include <libxml/tree.h>


XmlBboxCollection * xml_bbox_collection_make_from_doc(xmlDoc *document);


#endif
