#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <usb.h>
#include <stdint.h>
#include <string.h>
#include "check_value.h"
#include "handle_input.h"

/// defines ///
#define G560_VENDOR_ID   0x046d  // Logitech USB Vendor ID
#define G560_PRODUCT_ID  0x0a78  // G560 USB Product ID

#define DATA_SEND_LEN 48
#define MAX_ARR_LEN 256
#define NUM_OF_LED_AREAS 4

#define SET_COLOUR_TEMPATE "11ff043a%02x01%06x02"

#define INTERFACE_NUM 2

/// global variables ///
int chosen_colour = 0xffffff;
int c2 = 0xff0000;
int c3 = 0xff00ff;

int kernelDriverDetached     = 0;

void processMessage(const uint8_t*);

/// functions ///

int a2v(char c)
{
    if ((c >= '0') && (c <= '9'))
    {
        return c - '0';
    }
    if ((c >= 'a') && (c <= 'f'))
    {
        return c - 'a' + 10;
    }
    else return 0;
}

void unhexlify(char *hstr, int len) {
    char bstr[MAX_ARR_LEN] = {};
    memset(bstr, 0, MAX_ARR_LEN);
    char * pbstr = bstr;

    for (int i = 0; i < strlen(hstr); i += 2)
    {
        *pbstr++ = (a2v(hstr[i]) << 4) + a2v(hstr[i + 1]);
    }
    *pbstr++ = '\0';

    int i = 0;
    for (; i < len/2; i++)
    {
        hstr[i] = bstr[i];
    }
    hstr[i] = '\0';

    return;
}

static void send_ctrl_msg
        (libusb_device_handle *handle,
        unsigned int sel_led,
        unsigned int colour)
{
  uint16_t numBytes = 10;
  char dummy[MAX_ARR_LEN] = {};
  memset(dummy, 0, MAX_ARR_LEN);
  sprintf(dummy, "11ff043a%02x01%06x02", sel_led, colour);

  unhexlify(dummy, strlen(dummy));

  int ctr = 0;
  int val = -1;

  while ((val < 0) && (ctr < 30))
  {
    val = libusb_control_transfer(handle,
                    0x21,
                    0x09,
                    0x0211,
                    0x0002,
                    dummy,
                    numBytes,
                    0);

    if (val >= 0)
    {
      printf("%d bytes transmitted successfully.\n", numBytes);
    }
    else
    {
      fprintf(stderr, "\033[0;33mError couldn't send message to device.\033[0m\n");
      usleep(35000);
      ctr++;
      if(ctr < 30)
        fprintf(stderr, "\033[0;33mTrying again.\033[0m\n");
    }
  }

  printf("val is: %d, sel_led is: %02x, colour is: %06x\n", val, sel_led, colour);
}

int detach_send_reattach_driver(libusb_device_handle* handle, int n_colours,
                                int* result_buff)
{
  int res                      = 0;  /* return codes from libusb functions */
  kernelDriverDetached = 0;  /* Set to 1 if kernel driver detached */
  int numBytes                 = 0;  /* Actual bytes transferred. */


  /* Check whether a kernel driver is attached to interface #2. If so, we'll
   * need to detach it.
   */
  if (libusb_kernel_driver_active(handle, INTERFACE_NUM))
  {
    res = libusb_detach_kernel_driver(handle, INTERFACE_NUM);
    if (res == 0)
    {
      kernelDriverDetached = 1;
      fprintf(stderr, "detached kernel driver.\n");
    }
    else
    {
      fprintf(stderr, "Error detaching kernel driver.\n");
      return 1;
    }
  }
  else
  {
    printf("Kernel driver inactive.\n");
    return 1;
  }

  /* Claim interface. */
  res = libusb_claim_interface(handle, INTERFACE_NUM);
  if (res != 0)
  {
    fprintf(stderr, "Error claiming interface.\n");
    return 1;
  }

  if (result_buff[0] == 1)
  {
    // Do LED changes here
    for (int i = 2; i < (n_colours*2)+2; i += 2)
    {
      send_ctrl_msg(handle, result_buff[i], result_buff[i+1]);
    }
  }



  /* Release interface #2. */
  res = libusb_release_interface(handle, INTERFACE_NUM);
  if (0 != res)
  {
    fprintf(stderr, "Error releasing interface.\n");
  }

  /* If we detached a kernel driver from interface #0 earlier, we'll now
   * need to attach it again.  */
  if (kernelDriverDetached)
  {
    res = libusb_attach_kernel_driver(handle, INTERFACE_NUM);
    while (res < 0)
    {
      fprintf(stderr, "\033[1;31mError reattaching kernel driver: %d\033[0m\n", res);
      fprintf(stderr, "\033[0;33mTrying again.\033[0m\n");
      res = libusb_attach_kernel_driver(handle, INTERFACE_NUM);
    }
    fprintf(stderr, "reattached kernel driver.\n");
  }
}

void print_result_buffer(int argc, int * result_buffer)
{
  for (int i = 0; i < argc; i++)
  {
    printf("[%d] is: %d ", i, result_buffer[i]);
  }
  printf("\n");
}


/*--------------------------main function--------------------------------*/
int main(int argc, char** argv)
{
  int res                      = 0;  /* return codes from libusb functions */
  libusb_device_handle* handle = 0;  /* handle for USB device */
  libusb_context *context = NULL;
  int result_buffer[RESULT_BUFF_SIZE] = {};
  int r_val[1] = {0};
  memset(result_buffer, 0, RESULT_BUFF_SIZE);


  res = process_input(argc, argv, result_buffer, r_val);
  if (r_val[0])
  {
    return 1;
  }

  /* Init libusb */
  res = libusb_init(&context);
  if (res != 0)
  {
    fprintf(stderr, "Error initializing libusb.\n");
    return 1;
  }

  handle = libusb_open_device_with_vid_pid(context, G560_VENDOR_ID, G560_PRODUCT_ID);
  if (!handle)
  {
    fprintf(stderr, "Unable to open device.\n");
    return 1;
  }

  int num_colours = result_buffer[1];

  res = detach_send_reattach_driver(handle, num_colours, result_buffer);

  /* Shutdown libusb */
  libusb_exit(0);
  return 0;
}