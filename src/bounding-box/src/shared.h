#ifndef APP_BOUNDING_BOX_SHARED_H
#define APP_BOUNDING_BOX_SHARED_H


#include "../include/bounding-box.h"
#include <libxml/parser.h>
#include <libxml/tree.h>


#ifndef PROTECTED
#define PROTECTED
#endif


struct SvgDocument {
    xmlDoc xmlDoc;
};


PROTECTED void xml_bbox_collection_make_from_node(
    xmlNode *node,
    BboxCollection *collection,
    Transform *trans_acc
);


#endif
