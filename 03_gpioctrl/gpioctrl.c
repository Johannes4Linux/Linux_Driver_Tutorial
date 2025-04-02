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

#include <sys/gpio.h>  //  for gpio flags

#include <dev/gpio/gpiobusvar.h>
#include <dev/ofw/ofw_bus.h>

static gpio_pin_t led_pin = NULL;
static gpio_pin_t button_pin = NULL;

static int load_metadata(void) {
    // Add a sysctrl context and object identifier to the system control tree
    if (sysctl_ctx_init(&sysctl_ctx)) {
        return ENOMEM;
    }

    sysctl_tree = SYSCTL_ADD_NODE(&sysctl_ctx,
                                  SYSCTL_STATIC_CHILDREN(_kern),
                                  OID_AUTO,
                                  "gpioctrl",
                                  CTLFLAG_RW,
                                  0,
                                  "gpioctrl metadata");

    // Add metadata entries
    SYSCTL_ADD_STRING(&sysctl_ctx, SYSCTL_CHILDREN(sysctl_tree),
                      OID_AUTO, "author", CTLFLAG_RD,
                      "John Holloway", 0, "Module author");

    SYSCTL_ADD_STRING(&sysctl_ctx, SYSCTL_CHILDREN(sysctl_tree),
                      OID_AUTO, "description", CTLFLAG_RD,
                      "A simple module to turn on an LED and read from a pin",
                      0, "Module description");

    SYSCTL_ADD_STRING(&sysctl_ctx, SYSCTL_CHILDREN(sysctl_tree),
                      OID_AUTO, "license", CTLFLAG_RD,
                      "GPL", 0, "Module license");

    SYSCTL_ADD_STRING(&sysctl_ctx, SYSCTL_CHILDREN(sysctl_tree),
                      OID_AUTO, "version", CTLFLAG_RD,
                      "1.0", 0, "Module version");
    return 0;
}

static int init_module(void) {
    int error = 0;
    uint32_t led_number = 21;
    uint32_t button_number = 20;
    device_t gpio_dev;

    gpio_dev = devclass_get_device(devclass_find("gpiobus"), 0);

    if (!gpio_dev) {
        printf("no GPIO device found.\n");
        return ENXIO;
    } else {
        printf("GPIO device found: %s\n",
               device_get_nameunit(gpio_dev));


        if ((error = gpio_pin_get_by_bus_pinnum(
                gpio_dev,
                led_number,
                &led_pin)) == 0) {
            gpio_pin_setflags(led_pin, GPIO_PIN_OUTPUT);

            gpio_pin_set_active(led_pin, TRUE);
        } else {
            printf("error: %d\n", error);
            return error;
        }

        if ((error = gpio_pin_get_by_bus_pinnum(
                gpio_dev,
                button_number,
                &button_pin)) == 0) {
            gpio_pin_setflags(button_pin, GPIO_PIN_INPUT);

            bool active;
            if (gpio_pin_is_active(button_pin, &active) == 0) {
                printf("Button is %spressed", active ? "" : "not ");
            } else {
                printf("Failed to read button state\n");
            }


        } else {
            printf("error: %d\n", error);
            return error;
        }
    }

    return error;
}

static int loader(struct module *module, int what, void *arg) {
    int error = 0;


    switch (what) {
        case MOD_LOAD:
            printf("Module loaded into FreeBSD kernel\n");

            error = load_metadata();
            if (error != 0) {
                break;
            }

            error = init_module();

            break;
        case MOD_UNLOAD:
            printf("Module removed from the FreeBSD kernel\n");

            if (led_pin) {
                printf("releasing GPIO pin\n");
                gpio_pin_set_active(led_pin, FALSE);
                gpio_pin_release(led_pin);
                printf("pin released\n");
            }

            if (button_pin) {
                printf("releasting button pin\n");
                gpio_pin_release(button_pin);
                printf("pin released\n");
            }

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
    return (error);
}

static moduledata_t mod = {
        "gpioctrl",
        loader,
        NULL
};

DECLARE_MODULE(gpioctrl, mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE
);
