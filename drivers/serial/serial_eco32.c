#include <common.h>
#include <driver.h>
#include <init.h>
#include <malloc.h>
#include <io.h>

#define RX_CR   0x0
#define RX_DA   0x4
#define TX_CR   0x8
#define TX_DA   0xC
#define READY   0x1


struct eco32_serial_priv{
    struct console_device cdev;
    void __iomem* regs;
};


static int eco32_serial_setbaudrate(struct console_device *cdev, int baudrate)
{
    /* the baudrate is fixed */
    return 0;
}

static void eco32_serial_putc(struct console_device *cdev, char c)
{
    struct eco32_serial_priv *priv = container_of(cdev, struct eco32_serial_priv, cdev);

    while (!(readl(priv->regs + TX_CR) & READY));
    writel(c, priv->regs + TX_DA);
}

static int eco32_serial_getc(struct console_device *cdev)
{
    struct eco32_serial_priv *priv = container_of(cdev, struct eco32_serial_priv, cdev);

    while (!(readl(priv->regs + RX_CR) & READY));
    return readl(priv->regs + RX_DA);
}

static int eco32_serial_tstc(struct console_device *cdev)
{
    struct eco32_serial_priv *priv = container_of(cdev, struct eco32_serial_priv, cdev);

    return readl(priv->regs + RX_CR) & READY ? 1 : 0;
}

static int eco32_serial_probe(struct device_d *dev)
{
    struct resource* iores;
    struct console_device* cdev;
    struct eco32_serial_priv* priv;

    priv = xzalloc(sizeof(*priv));
    cdev = &priv->cdev;

    iores = dev_request_mem_resource(dev, 0);
    if (IS_ERR(iores))
        return PTR_ERR(iores);
    priv->regs = IOMEM(iores->start);

    cdev->dev = dev;
    cdev->tstc = eco32_serial_tstc;
    cdev->putc = eco32_serial_putc;
    cdev->getc = eco32_serial_getc;
    cdev->setbrg = eco32_serial_setbaudrate;

    console_register(cdev);

    return 0;
}

static struct driver_d eco32_serial_driver = {
        .name  = "eco32_serial",
        .probe = eco32_serial_probe,
};
console_platform_driver(eco32_serial_driver);
