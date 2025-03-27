/*
 *  Copyright [2025] John Holloway
 *  Learning exercise. Use at your own risk
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/sysctl.h>  //  required for the metadata

static struct sysctl_ctx_list sysctl_ctx;
static struct sysctl_oid *sysctl_tree;

static int load_metadata(void) {
    // Add a sysctrl context and object identifier to the system control tree
    if (sysctl_ctx_init(&sysctl_ctx)) {
        return ENOMEM;
    }

    sysctl_tree = SYSCTL_ADD_NODE(&sysctl_ctx,
                                  SYSCTL_STATIC_CHILDREN(_kern),
                                  OID_AUTO,
                                  "better_hello",
                                  CTLFLAG_RW,
                                  0,
                                  "better_hello metadata");

    // Add metadata entries
    SYSCTL_ADD_STRING(&sysctl_ctx, SYSCTL_CHILDREN(sysctl_tree),
                      OID_AUTO, "author", CTLFLAG_RD,
                      "John Holloway", 0, "Module author");

    SYSCTL_ADD_STRING(&sysctl_ctx, SYSCTL_CHILDREN(sysctl_tree),
                      OID_AUTO, "description", CTLFLAG_RD,
                      "A simple Hello World kernel module",
                      0, "Module description");

    SYSCTL_ADD_STRING(&sysctl_ctx, SYSCTL_CHILDREN(sysctl_tree),
                      OID_AUTO, "license", CTLFLAG_RD,
                      "BSD", 0, "Module license");

    SYSCTL_ADD_STRING(&sysctl_ctx, SYSCTL_CHILDREN(sysctl_tree),
                      OID_AUTO, "version", CTLFLAG_RD,
                      "1.0", 0, "Module version");
    return 0;
}

static int loader(struct module *module, int what, void *arg) {
        int error = 0;

        switch (what) {
                case MOD_LOAD:
                        printf("Module loaded into FreeBSD kernel\n"
                               "Use sysctl to get the kernel metadata\n");
                        error = load_metadata();
                        break;

                case MOD_UNLOAD:
                        printf("Module removed from the FreeBSD kernel\n");

                        // If we do not free the system call context
                        // we end up with dangling pointers in the kernel
                if (sysctl_ctx_free(&sysctl_ctx)) {
                    printf("Can't free the system call context\n");
                    error = ENOTEMPTY;
                } else {
                    printf("Metadata has been removed from sysctl tree\n");
                }

                        break;
                default:
                        error = EOPNOTSUPP;
                        break;
        }
        return(error);
}

static moduledata_t mod = {
        "better_hello",
        loader,
        NULL
};

DECLARE_MODULE(hello, mod, SI_SUB_KLD, SI_ORDER_ANY);

