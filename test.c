#include "service/util/service_free_list.h"
#include "service/util/service_rb_tree.h"
#include "service/util/service_object.h"
#include "service/mca/mca.h"
#include "service/mca/base/base.h"
#include "service/threads/mutex.h"
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
    OBJ_CONSTRUCT(&list, service_free_list_t);
    service_free_list_init_ex_new(&list, 10, 32, OBJ_CLASS(service_mutex_t),
        sizeof(service_mutex_t), 32, 10, 20, 5, NULL,NULL,
        NULL, NULL, NULL, 0, dummy_progress);
    CONSTRUCT_RB_TREE(&tree, dummy_progress);
    list_d = OBJ_NEW(service_free_list_t);
    service_free_list_init_ex_new(list_d, 10, 32, OBJ_CLASS(service_mutex_t),
                    sizeof(service_mutex_t), 32, 10, 20, 5, NULL,NULL,
                            NULL, NULL, NULL, 0, dummy_progress);
    service_rb_tree_new(&tree_d, dummy_progress);
    printf("%s:%d\n", __FILE__, __LINE__);

    list.fl_condition.ccs_progress_fn();
    tree.free_list.fl_condition.ccs_progress_fn();
    list_d->fl_condition.ccs_progress_fn();
    tree_d->free_list.fl_condition.ccs_progress_fn();

    ccs_mca_service_install_dirs_t id = {"dir1","dir2","dir3"};
    ccs_mca_base_open(id);
    printf("all done\n");
    return 0;
}
