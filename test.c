#include "service/util/service_free_list.h"
#include "service/util/service_rb_tree.h"
#include "service/util/service_object.h"
#include <stdio.h>

static int dummy_progress(void)
{
    printf("running dummy constructor\n");
    return 0;
}

int main(int argc, char **argv)
{
    printf("starting\n");
    service_free_list_t list;
    service_rb_tree_t tree;
    service_free_list_t *list_d;
    service_rb_tree_t *tree_d;
    CONSTRUCT_SERVICE_FREE_LIST(&list, dummy_progress);
    CONSTRUCT_RB_TREE(&tree, dummy_progress);
    service_free_list_new(&list_d, dummy_progress);
    service_rb_tree_new(&tree_d, dummy_progress);

    list.fl_condition.ccs_progress_fn();
    tree.free_list.fl_condition.ccs_progress_fn();
    list_d->fl_condition.ccs_progress_fn();
    tree_d->free_list.fl_condition.ccs_progress_fn();

    printf("all done\n");
    return 0;
}
