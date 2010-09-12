#include <linux/module.h>

#ifdef CONFIG_USB_SUPPORT
#include <plat/regs-otg.h>
#include <plat/pll.h>
#include <plat/irqs.h>
#include <plat/regs-clock.h>
#include <linux/usb/ch9.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <mach/map.h>
#include <linux/clk.h>
#include <asm/io.h>
#include <asm/delay.h>

/* S3C_USB_CLKSRC 0: EPLL 1: CLK_48M */
// #define S3C_USB_CLKSRC<>1
#define OTGH_PHY_CLK_VALUE      (0x22)  /* UTMI Interface, otg_phy input clk 12Mhz Oscillator */
#endif

#ifdef CONFIG_USB_SUPPORT

//these devices are from mach-s5pc110/include/mach/map.h
#define S5PC11X_PA_USB_OHCI     (0xEC300000)
#define S5PC11X_PA_USB_EHCI     (0xEC200000)

#define S5PC11X_SZ_USB_OHCI     SZ_1M
#define S5PC11X_SZ_USB_EHCI     SZ_1M




// these four structs came from plat-s3c/devs.c
/* USB Host Controller OHCI */
static struct resource s3c_usb__ohci_resource[] = {
        [0] = {
                .start = S5PC11X_PA_USB_OHCI ,
                .end   = S5PC11X_PA_USB_OHCI  + S5PC11X_SZ_USB_OHCI - 1,
                .flags = IORESOURCE_MEM,
        },
        [1] = {
                .start = IRQ_UHOST,
                .end   = IRQ_UHOST,
                .flags = IORESOURCE_IRQ,
        }
};

static u64 s3c_device_usb_ohci_dmamask = 0xffffffffUL;

struct platform_device s3c_device_usb_ohci = {
        .name        = "s5pc110-ohci",
        .id            = -1,
        .num_resources    = ARRAY_SIZE(s3c_usb__ohci_resource),
        .resource        = s3c_usb__ohci_resource,
        .dev          = {
                .dma_mask = &s3c_device_usb_ohci_dmamask,
                .coherent_dma_mask = 0xffffffffUL
        }
};

EXPORT_SYMBOL(s3c_device_usb_ohci);

/* USB Host Controller EHCI */

static struct resource s3c_usb__ehci_resource[] = {
        [0] = {
                .start = S5PC11X_PA_USB_EHCI ,
                .end   = S5PC11X_PA_USB_EHCI  + S5PC11X_SZ_USB_EHCI - 1,
                .flags = IORESOURCE_MEM,
        },
        [1] = {
                .start = IRQ_UHOST,
                .end   = IRQ_UHOST,
                .flags = IORESOURCE_IRQ,
        }
};

static u64 s3c_device_usb_ehci_dmamask = 0xffffffffUL;

struct platform_device s3c_device_usb_ehci = {
        .name        = "s5pc110-ehci",
        .id            = -1,
        .num_resources    = ARRAY_SIZE(s3c_usb__ehci_resource),
        .resource        = s3c_usb__ehci_resource,
        .dev          = {
                .dma_mask = &s3c_device_usb_ehci_dmamask,
                .coherent_dma_mask = 0xffffffffUL
        }
};

EXPORT_SYMBOL(s3c_device_usb_ehci);




// /* Initializes OTG Phy. */
// void otg_phy_init(void) {
//         writel(readl(S5P_USB_PHY_CONTROL)|(0x1<<0), S5P_USB_PHY_CONTROL); /*USB PHY0 Enable */
//         writel((readl(S3C_USBOTG_PHYPWR)&~(0x3<<3)&~(0x1<<0))|(0x1<<5), S3C_USBOTG_PHYPWR);
//         writel((readl(S3C_USBOTG_PHYCLK)&~(0x5<<2))|(0x3<<0), S3C_USBOTG_PHYCLK);
//         writel((readl(S3C_USBOTG_RSTCON)&~(0x3<<1))|(0x1<<0), S3C_USBOTG_RSTCON);
//         udelay(10);
//         writel(readl(S3C_USBOTG_RSTCON)&~(0x7<<0), S3C_USBOTG_RSTCON);
//         udelay(10);
// 
// }
// EXPORT_SYMBOL(otg_phy_init);
// 
// /* USB Control request data struct must be located here for DMA transfer */
// struct usb_ctrlrequest usb_ctrl __attribute__((aligned(8)));
// EXPORT_SYMBOL(usb_ctrl);
// 
// /* OTG PHY Power Off */
// void otg_phy_off(void) {
//         writel(readl(S3C_USBOTG_PHYPWR)|(0x3<<3), S3C_USBOTG_PHYPWR);
//         writel(readl(S5P_USB_PHY_CONTROL)&~(1<<0), S5P_USB_PHY_CONTROL);
// 
// }
// EXPORT_SYMBOL(otg_phy_off);

// void usb_host_clk_en(void) {
// 
// }
// 
// EXPORT_SYMBOL(usb_host_clk_en);

void usb_host_phy_init(void)
{
   struct clk *otg_clk;

   otg_clk = clk_get(NULL, "otg");
   clk_enable(otg_clk);

   if(readl(S5P_USB_PHY_CONTROL)&(0x1<<1)){
           if (readl(S5P_RST_STAT)&(0x1<<1)) // if nwRESET, force to power-up.
                   printk("USB_CONTROL= 0x%x, RST_STAT=%x\n", readl(S5P_USB_PHY_CONTROL), readl(S5P_RST_STAT));
           else
                   return;
   }

   writel(readl(S5P_USB_PHY_CONTROL)|(0x1<<1), S5P_USB_PHY_CONTROL);
   writel((readl(S3C_USBOTG_PHYPWR)&~(0x1<<7)&~(0x1<<6))|(0x1<<8)|(0x1<<5), S3C_USBOTG_PHYPWR);
   writel((readl(S3C_USBOTG_PHYCLK)&~(0x1<<7))|(0x3<<0), S3C_USBOTG_PHYCLK);
   writel((readl(S3C_USBOTG_RSTCON))|(0x1<<4)|(0x1<<3), S3C_USBOTG_RSTCON);
   udelay(10);
   writel(readl(S3C_USBOTG_RSTCON)&~(0x1<<4)&~(0x1<<3), S3C_USBOTG_RSTCON);
   udelay(10);
}
EXPORT_SYMBOL(usb_host_phy_init);

void usb_host_phy_off(void)
{
   writel(readl(S3C_USBOTG_PHYPWR)|(0x1<<7)|(0x1<<6), S3C_USBOTG_PHYPWR);
   writel(readl(S5P_USB_PHY_CONTROL)&~(1<<1), S5P_USB_PHY_CONTROL);
}
EXPORT_SYMBOL(usb_host_phy_off);
#endif /* CONFIG_USB_SUPPORT */


static struct platform_device *usb_host_devices[] __initdata = {
	&s3c_device_usb_ohci,
	&s3c_device_usb_ehci,
};

static int mach_jupiter_usb_init(void)
  {
	platform_add_devices(usb_host_devices, ARRAY_SIZE(usb_host_devices));
  return 0;
  }

static void mach_jupiter_usb_cleanup(void)
  {
  int i;
  for (i=0;i<ARRAY_SIZE(usb_host_devices);i++)
    {
    platform_device_unregister(usb_host_devices[i]);
    }
  }

module_init(mach_jupiter_usb_init);
module_exit(mach_jupiter_usb_cleanup);

MODULE_DESCRIPTION("Mach S5PC110 Jupiter Board USB phy module");
MODULE_AUTHOR ("rtm");
MODULE_LICENSE ("GPL");
