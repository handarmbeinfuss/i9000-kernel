struct usb_cable_notify {
  void (*handler)(int state);
  struct usb_cable_notify *next;
};

extern void usb_cable_notify_register(struct usb_cable_notify *ucn);
extern void usb_cable_notify_unregister(struct usb_cable_notify *ucn);
