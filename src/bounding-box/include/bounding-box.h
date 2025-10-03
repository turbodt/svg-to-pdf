#ifndef APP_BOUNDING_BOX_H
#define APP_BOUNDING_BOX_H


#include <geometry.h>
#include <custom-svg.h> // for bbox computations


// Collection

typedef struct BboxItem {
    void const *id;
    Box2D bbox;
} BboxItem;
typedef struct BboxCollection BboxCollection;


BboxCollection * bbox_collection_make(void);
void bbox_collection_destroy(BboxCollection *);


unsigned int bbox_collection_get_count(BboxCollection const *);
BboxItem const * bbox_collection_getc(
    BboxCollection const *,
    unsigned int index
);


int bbox_collection_append(BboxCollection *, void const *id, Box2D bbox);
int bbox_collection_remove(BboxCollection *, unsigned int index);

void bbox_collection_sort(
    BboxCollection *,
    int(*comparator)(BboxItem const *, BboxItem const *)
);
void bbox_collection_sort_area(BboxCollection *);
void bbox_collection_sort_dim(BboxCollection *);
BboxCollection *bbox_collection_filter_by_dimensions(
    BboxCollection const *collection,
    Size2D target_size,
    float tolerance
);
BboxCollection *bbox_collection_filter_by_dimensions_not(
    BboxCollection const *collection,
    Size2D target_size,
    float tolerance
);
BboxCollection *bbox_collection_filter_intersecting(
    BboxCollection const *collection,
    BboxItem const *item
);


// Document


typedef struct SvgDocument SvgDocument;


void bbox_clean_up(void);
SvgDocument * bbox_svg_doc_make_from_file(char const *filename);
SvgDocument * bbox_svg_doc_make_from_subset(
    SvgDocument *,
    BboxCollection const *to_keep
);
void bbox_svg_doc_destroy(SvgDocument *doc);
BboxCollection * bbox_collection_make_from_doc(SvgDocument *doc);
void bbox_svg_doc_save_file(SvgDocument *doc, char const *filename);
void bbox_svg_doc_set_viewbox(SvgDocument *doc, Box2D);

#endif
