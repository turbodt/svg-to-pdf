#include "./shared.h"
#include <stdlib.h>


struct BboxCollection {
    BboxItem *items;
    unsigned int count;
    unsigned int capacity;
};
typedef BboxCollection Impl;


static int ensure_capacity(Impl *, unsigned int target_capacity);
static int cmp_area(BboxItem const *, BboxItem const *);
static int cmp_dimensions(BboxItem const *, BboxItem const *);


BboxCollection * bbox_collection_make(void) {
    Impl *impl = malloc(sizeof(Impl));
    if (!impl) {
        return NULL;
    }

    impl->items = NULL;
    impl->count = 0;
    impl->capacity = 0;

    return impl;
}


void bbox_collection_destroy(BboxCollection *impl) {
    if (impl->items) {
        free(impl->items);
        impl->items = NULL;
    }
    impl->count = 0;
    impl->capacity = 0;
    free(impl);
};


inline unsigned int bbox_collection_get_count(BboxCollection const *impl) {
    return impl->count;
};


inline BboxItem const * bbox_collection_getc(
    BboxCollection const *impl,
    unsigned int index
) {
    if (index >= impl->count) {
        return NULL;
    }
    return &impl->items[index];
};


int bbox_collection_append(
    BboxCollection *impl,
    void const *id,
    Box2D bbox
) {
    unsigned int current_count = bbox_collection_get_count(impl);
    int err = ensure_capacity(impl, current_count + 1);
    if (err) {
        return err;
    }

    impl->items[impl->count] = (BboxItem){
        .id = id,
        .bbox = bbox
    };
    impl->count++;

    return 0;
};


void bbox_collection_sort(
    BboxCollection *impl,
    int(*cmp)(BboxItem const *, BboxItem const *)
) {
    qsort(
        impl->items,
        impl->count,
        sizeof(BboxItem),
        (int(*)(void const *, void const*))cmp
    );
};


inline void bbox_collection_sort_area(BboxCollection *impl) {
    bbox_collection_sort(impl, cmp_area);
};


inline void bbox_collection_sort_dim(BboxCollection *impl) {
    bbox_collection_sort(impl, cmp_dimensions);
};


int ensure_capacity(Impl *impl, unsigned int target_capacity) {
    if (target_capacity <= impl->capacity) {
        return 0;
    }

    unsigned int new_capacity;
    if (impl->capacity == 0) {
        new_capacity = 8;
    }
    while (new_capacity > 8 && new_capacity >= 2*target_capacity) {
        new_capacity /= 2;
    }
    while (new_capacity < target_capacity) {
        new_capacity *= 2;
    }

    BboxItem *new_ptr;
    if (impl->capacity == 0) {
        new_ptr = malloc(sizeof(BboxItem)*target_capacity);
    } else {
        new_ptr = realloc(impl->items, sizeof(BboxItem)*target_capacity);
    }
    if (!new_ptr) {
        return 1;
    }
    impl->items = new_ptr;
    impl->capacity = target_capacity;

    return 0;
};


int cmp_area(BboxItem const *a, BboxItem const *b) {
    return (a->bbox.size.width * a->bbox.size.height)
        - (b->bbox.size.width * b->bbox.size.height);
};


int cmp_dimensions(BboxItem const *a, BboxItem const *b) {
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
