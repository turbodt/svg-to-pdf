#include "./shared.h"


static int matches_size(BboxItem const *, Size2D target_size, float tol);


BboxCollection * bbox_collection_filter_by_dimensions(
    BboxCollection const *collection,
    Size2D target_size,
    float tol
) {
    BboxCollection *new_coll = bbox_collection_make();
    if (!new_coll) {
        return NULL;
    }
    unsigned int const item_count = bbox_collection_get_count(collection);
    for (unsigned int i = 0; i < item_count; i++) {
        BboxItem const *item = bbox_collection_getc(collection, i);
        if (!matches_size(item, target_size, tol)) {
            continue;
        }

        bbox_collection_append(new_coll, item->id, item->bbox);
    }

    return new_coll;
};


BboxCollection * bbox_collection_filter_by_dimensions_not(
    BboxCollection const *collection,
    Size2D target_size,
    float tol
) {
    BboxCollection *new_coll = bbox_collection_make();
    if (!new_coll) {
        return NULL;
    }
    unsigned int const item_count = bbox_collection_get_count(collection);
    for (unsigned int i = 0; i < item_count; i++) {
        BboxItem const *item = bbox_collection_getc(collection, i);
        if (matches_size(item, target_size, tol)) {
            continue;
        }

        bbox_collection_append(new_coll, item->id, item->bbox);
    }

    return new_coll;
};


BboxCollection *bbox_collection_filter_intersecting(
    BboxCollection const *collection,
    BboxItem const *container_item
) {
    BboxCollection *new_coll = bbox_collection_make();
    if (!new_coll) {
        return NULL;
    }
    unsigned int const item_count = bbox_collection_get_count(collection);
    for (unsigned int i = 0; i < item_count; i++) {
        BboxItem const *item = bbox_collection_getc(collection, i);
        if (container_item->id == item->id) {
            continue;
        }
        if (!geo_boxes_intersect(container_item->bbox, item->bbox)) {
            continue;
        }
        bbox_collection_append(new_coll, item->id, item->bbox);
    }

    return new_coll;
};


int matches_size(BboxItem const *item, Size2D target_size, float tol) {
    if (
        item->bbox.size.width * (1+tol) < target_size.width
        || target_size.width * (1+tol) < item->bbox.size.width
    ) {
        return 0;
    }

    if (
        item->bbox.size.height * (1+tol) < target_size.height
        || target_size.height * (1+tol) < item->bbox.size.height
    ) {
        return 0;
    }
    return 1;
};
