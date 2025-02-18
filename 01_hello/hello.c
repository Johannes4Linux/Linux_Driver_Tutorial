#include <sys/types.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

static int loader(struct module *module, int what, void *arg)
{
        int error = 0;

        switch(what) {
                case MOD_LOAD:
                        printf("Module loaded into FreeBSD kernel\n");
                        break;
                case MOD_UNLOAD:
                        printf("Module removed from the FreeBSD kernel\n");
                        break;
                default:
                        error = EOPNOTSUPP;
                        break;
        }
        return(error);
}

static moduledata_t mod = {
        "hello",
        loader,
        NULL
};

DECLARE_MODULE(hello, mod, SI_SUB_KLD, SI_ORDER_ANY);
jholloway@BSD-PI4:~/01_hello $ cat Makefile 
SRCS=hello.c
KMOD=hello

.include <bsd.kmod.mk>

