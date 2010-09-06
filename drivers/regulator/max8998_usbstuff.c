#include <linux/module.h>
#include <mach/battery.h>
#include <mach/max8998_function.h>

extern int set_tsp_for_ta_detect(int state);
extern u8 FSA9480_Get_I2C_USB_Status(void);

extern charging_device_type curent_device_type;
extern u8 MicroTAstatus;
void maxim_vac_connect_function(void *data)
  {
	u8 UsbStatus=0;
    UsbStatus = FSA9480_Get_I2C_USB_Status();
    if(UsbStatus){
      //printk("maxim_USB_connect~~~ \n");
      curent_device_type = PM_CHARGER_USB_INSERT;
    }
    else{
      //printk("maxim_TA_connect~~~ \n");
      curent_device_type = PM_CHARGER_TA;
      MicroTAstatus = 1;
      set_tsp_for_ta_detect(1);
    }
  }
static maxusb_function mvc = {
  .kind=mufMAXIM_VAC_CONNECT_FUNC,
  .afunc=maxim_vac_connect_function,
  .next=NULL
  };

void maxim_chg_status_function(void *data)
  {
		if(FSA9480_Get_I2C_USB_Status()){
			//printk("maxim_USB_connect~~~ \n");
			curent_device_type = PM_CHARGER_USB_INSERT;
		}
		else{
			//printk("maxim_TA_connect~~~ \n");
			curent_device_type = PM_CHARGER_TA;
			MicroTAstatus = 1;
		}
  }
static maxusb_function mcs = {
  .kind=mufMAXIM_CHG_STATUS_FUNC,
  .afunc=maxim_chg_status_function,
  .next=NULL
  };

void set_MTA0(void *data)
  {
  MicroTAstatus = 0;
  }
static maxusb_function sMTA0 = {
  .kind=mufSET_MTA0,
  .afunc=set_MTA0,
  .next=NULL
  };


extern int  FSA9480_PMIC_CP_USB(void);
extern int mtp_mode_on;
extern int askonstatus;
void maxim_charging_control_function(void *data)
  {
	byte reg_buff[2];
	int uicharging=*((int*)data);
	int value;
		value = FSA9480_PMIC_CP_USB();
		if(uicharging)
			reg_buff[0] = (0x1<<5) |(0x3 << 3) |(0x2<<0) ; // CHG_TOPOFF_TH=15%, CHG_RST_HYS=disable, AC_FCGH= 475mA
		else
			reg_buff[0] = (0x3 <<5) |(0x3 << 3) |(0x2<<0) ; // CHG_TOPOFF_TH=25%, CHG_RST_HYS=disable, AC_FCGH= 475mA
		if(value)
		{
			if (askonstatus||mtp_mode_on){
				reg_buff[1] = (0x1<<6) |(0x2<<4) | (0x0<<3) | (0x0<<1) | (0x0<<0); //ESAFEOUT1,2= 01, FCHG_TMR=7Hr, MBAT_REG_TH=4.2V, MBATT_THERM_REG=105C
				printk("[Max8998_function]AP USB Power OFF, askon: %d, mtp : %d\n",askonstatus,mtp_mode_on);
				}
			else{
				reg_buff[1] = (0x2<<6) |(0x2<<4) | (0x0<<3) | (0x0<<1) | (0x0<<0); //ESAFEOUT1,2= 10, FCHG_TMR=7Hr, MBAT_REG_TH=4.2V, MBATT_THERM_REG=105C
				printk("[Max8998_function]AP USB Power ON, askon: %d, mtp : %d\n",askonstatus,mtp_mode_on);
				}
		}
		else
			reg_buff[1] = (0x2<<5) |(0x2<<4) | (0x0<<3) | (0x0<<1) | (0x0<<0); //ESAFEOUT1,2= 01, FCHG_TMR=7Hr, MBAT_REG_TH=4.2V, MBATT_THERM_REG=105C
		Set_MAX8998_PM_ADDR(CHGR1, reg_buff, 2); 
  }
static maxusb_function mcc = {
  .kind=mufMACIM_CHARGING_CONTROL,
  .afunc=maxim_charging_control_function,
  .next=NULL
  };

extern u8 FSA9480_Get_JIG_Status(void);
void FSA9480_Get_JIG_Status_func(void *data)
  {
  u8 i=FSA9480_Get_JIG_Status();
  *((u8*)data)=i;
  }
static maxusb_function gjs = {
  .kind=mufFSA9480_GET_JIG_STATUS,
  .afunc=FSA9480_Get_JIG_Status_func,
  .next=NULL
  };

extern int log_via_usb;
void log_via_usb_func(void *data)
  {
  *((int*)data)=log_via_usb;
  }
static maxusb_function lvu = {
  .kind=mufLOG_VIA_USB,
  .afunc=log_via_usb_func,
  .next=NULL
  };

// drivers/regulator/max8998_function 
// D maxim_vac_connect,
// D maxim_chg_status,
// D maxim_vac_disconnect,
// D maxim_charging_control
// D drivers/power/s5pc110_battery.c: s3c_bat_check_v_f
// D kernel/printk.c printk

static int register_module(void)
  {
  register_muf(&mvc);
  register_muf(&mcs);
  register_muf(&sMTA0);
  register_muf(&mcc);
  register_muf(&gjs);
  register_muf(&lvu);
  return 0;
  }

static void unregister_module(void)
  {
  unregister_muf(&mvc);
  unregister_muf(&mcs);
  unregister_muf(&sMTA0);
  unregister_muf(&mcc);
  unregister_muf(&gjs);
  unregister_muf(&lvu);
  }

module_init(register_module);
module_exit(unregister_module);

MODULE_DESCRIPTION("MAXIM 8998 voltage regulator USB gadget dependent part");
MODULE_AUTHOR("rtm");
MODULE_LICENSE("GPL");

