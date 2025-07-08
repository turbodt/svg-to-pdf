#include "./xml-bbox-collection.h"
#include <stdlib.h>


struct XmlBboxCollection {
    XmlBboxItem *items;
    unsigned int count;
    unsigned int capacity;
};
typedef XmlBboxCollection Impl;


static int ensure_capacity(Impl *impl, unsigned int target_capacity);
static int cmp_area(XmlBboxItem const *, XmlBboxItem const *);
static int cmp_dimensions(XmlBboxItem const *, XmlBboxItem const *);


XmlBboxCollection * xml_bbox_collection_make(void) {
    Impl *impl = malloc(sizeof(Impl));
    if (!impl) {
        return NULL;
    }

    impl->items = NULL;
    impl->count = 0;
    impl->capacity = 0;

    return impl;
}


void xml_bbox_collection_destroy(XmlBboxCollection *impl) {
    impl->items = NULL;
    impl->count = 0;
    impl->capacity = 0;
    free(impl);
};


inline unsigned int xml_bbox_collection_get_count(
    XmlBboxCollection const *impl
) {
    return impl->count;
};


inline XmlBboxItem const * xml_bbox_collection_getc(
    XmlBboxCollection const *impl,
    unsigned int index
) {
    if (index >= impl->count) {
        return NULL;
    }
    return &impl->items[index];
};


int xml_bbox_collection_append(
    XmlBboxCollection *impl,
    xmlNode const *node,
    Box2D bbox
) {
    unsigned int current_count = xml_bbox_collection_get_count(impl);
    int err = ensure_capacity(impl, current_count + 1);
    if (err) {
        return err;
    }

    impl->items[impl->count] = (XmlBboxItem){
        .node = node,
        .bbox = bbox
    };
    impl->count++;

    return 0;
};


void xml_bbox_collection_sort(
    XmlBboxCollection *impl,
    int(*cmp)(XmlBboxItem const *, XmlBboxItem const *)
) {
    qsort(
        impl->items,
        impl->count,
        sizeof(XmlBboxItem),
        (int(*)(void const *, void const*))cmp
    );
};


inline void xml_bbox_collection_sort_area(XmlBboxCollection *impl) {
    xml_bbox_collection_sort(impl, cmp_area);
};


inline void xml_bbox_collection_sort_dim(XmlBboxCollection *impl) {
    xml_bbox_collection_sort(impl, cmp_dimensions);
};


int ensure_capacity(Impl *impl, unsigned int target_capacity) {
    if (target_capacity <= impl->capacity) {
        return 0;
    }

    unsigned int new_capacity;
    if (impl->capacity == 0) {
        new_capacity = 8;
    } else {
        new_capacity = 2*impl->capacity;
    }
    while (new_capacity < target_capacity) {
        new_capacity *= 2;
    }

    XmlBboxItem *new_ptr;
    if (impl->capacity == 0) {
        new_ptr = malloc(sizeof(XmlBboxItem)*target_capacity);
    } else {
        new_ptr = realloc(impl->items, sizeof(XmlBboxItem)*target_capacity);
    }
    if (!new_ptr) {
        return 1;
    }
    impl->items = new_ptr;
    impl->capacity = target_capacity;

    return 0;
};


int cmp_area(XmlBboxItem const *a, XmlBboxItem const *b) {
    return (a->bbox.size.width * a->bbox.size.height)
        - (b->bbox.size.width * b->bbox.size.height);
};


int cmp_dimensions(XmlBboxItem const *a, XmlBboxItem const *b) {
    if (a->bbox.size.width > b->bbox.size.width) {
        return -1;
    } else if (a->bbox.size.width < b->bbox.size.width) {
        return 1;
    }
    if (a->bbox.size.height > b->bbox.size.height) {
        return -1;
    } else if (a->bbox.size.height < b->bbox.size.height) {
        return 1;
    }
    return 0;
};
