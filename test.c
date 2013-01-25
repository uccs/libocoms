#include "service/util/service_free_list.h"
#include "service/util/service_object.h"
#include <stdio.h>

static int dummy_progress(void)
{
    printf("running dummy constructor");
    return 0;
}

int main(int argc, char **argv)
{
    printf("starting\n");
    service_free_list_t list;
    CONSTRUCT_SERVICE_FREE_LIST(&list, dummy_progress);
    printf("all done\n");
    return 0;
}
