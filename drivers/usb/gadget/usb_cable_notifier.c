
#include <linux/module.h>
#include <linux/kernel.h>

#include "usb_cable_notifier.h"

#define DRIVER_DESC             "Wrapper around s3c_usb_cable to allow s3c_udc_otg to be modularized"
#define DRIVER_VERSION          "23 Aug 2010"


int BOOTUP = 1; // Booting ������ �ƴ��� �Ǵ��ϴ� ����, connectivity_switching_init �� �Ҹ� �Ŀ� 0�� ����.
EXPORT_SYMBOL(BOOTUP);

static struct usb_cable_notify *notify_list=NULL;

void usb_cable_notify_register(struct usb_cable_notify *ucn)
  {
  struct usb_cable_notify *runner=notify_list;
  ucn->next=NULL;
  if (runner==NULL)
    {
    notify_list=ucn;
    }
  else
    {
    while (runner->next!=NULL)
      {
      runner=runner->next;
      }
    runner->next=ucn;
    }
  }
EXPORT_SYMBOL(usb_cable_notify_register);

void usb_cable_notify_unregister(struct usb_cable_notify *ucn)
  {
  struct usb_cable_notify *runner=notify_list;
  if (runner!=NULL)
    {
    if (runner==ucn)
      {
      notify_list=notify_list->next;
      }
    else
      {
      while ((runner->next!=NULL) && (runner->next!=ucn))
        {
        runner=runner->next;
        }
      if (runner->next==ucn)
        {
        runner->next=ucn->next;
        }
      }
    }
  }
EXPORT_SYMBOL(usb_cable_notify_unregister);

int s3c_usb_cable(int connected)
  {
  struct usb_cable_notify *runner=notify_list;
  while (runner!=NULL)
    {
    if (runner->handler!=NULL)
      {
      runner->handler(connected);
      }
    runner=runner->next;
    }
  return 0;
  }
EXPORT_SYMBOL(s3c_usb_cable);


static int __init ucn_init(void)
{
        return 0;
}

static void __exit ucn_exit(void)
{
}

module_init(ucn_init);
module_exit(ucn_exit);

MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR("WK");
MODULE_LICENSE("GPL");
