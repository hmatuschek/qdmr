/*
 * HID routines for Linux, via libusb-1.0.
 *
 * Copyright (C) 2018 Serge Vakulenko, KK6ABQ
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>
#include "util.h"

static libusb_context *ctx = NULL;          // libusb context
static libusb_device_handle *dev;           // libusb device
static struct libusb_transfer *transfer;    // async transfer descriptor
static unsigned char receive_buf[42];       // receive buffer
static volatile int nbytes_received = 0;    // receive result

#define HID_INTERFACE   0                   // interface index
#define TIMEOUT_MSEC    500                 // receive timeout

//
// Callback function for asynchronous receive.
// Needs to fill the receive_buf and set nbytes_received.
//
static void read_callback(struct libusb_transfer *t)
{
    switch (t->status) {
    case LIBUSB_TRANSFER_COMPLETED:
        //fprintf(stderr, "%s: Transfer complete, %d bytes\n", __func__, t->actual_length);
        memcpy(receive_buf, t->buffer, t->actual_length);
        nbytes_received = t->actual_length;
        break;

    case LIBUSB_TRANSFER_CANCELLED:
        //fprintf(stderr, "%s: Transfer cancelled\n", __func__);
        nbytes_received = LIBUSB_ERROR_INTERRUPTED;
        return;

    case LIBUSB_TRANSFER_NO_DEVICE:
        //fprintf(stderr, "%s: No device\n", __func__);
        nbytes_received = LIBUSB_ERROR_NO_DEVICE;
        return;

    case LIBUSB_TRANSFER_TIMED_OUT:
        //fprintf(stderr, "%s: Timeout (normal)\n", __func__);
        nbytes_received = LIBUSB_ERROR_TIMEOUT;
        break;

    default:
        //fprintf(stderr, "%s: Unknown transfer code: %d\n", __func__, t->status);
        nbytes_received = LIBUSB_ERROR_IO;
   }
}

//
// Write data to the device and receive reply.
// Return negative status on error.
// Return received byte count of success.
// On timeout, repeat the transaction.
// Need to use callback for receive interrupt transfer.
//
static int write_read(const unsigned char *data, unsigned length, unsigned char *reply, unsigned rlength)
{
    if (! transfer) {
        // Allocate transfer descriptor on first invocation.
        transfer = libusb_alloc_transfer(0);
    }
    libusb_fill_interrupt_transfer(transfer, dev,
        LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_IN,
        reply, rlength, read_callback, 0, TIMEOUT_MSEC);
again:
    nbytes_received = 0;
    libusb_submit_transfer(transfer);

    int result = libusb_control_transfer(dev,
        LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_OUT,
        0x09/*HID Set_Report*/, (2/*HID output*/ << 8) | 0,
        HID_INTERFACE, (unsigned char*)data, length, TIMEOUT_MSEC);

    if (result < 0) {
        fprintf(stderr, "Error %d transmitting data via control transfer: %s\n",
            result, libusb_strerror(result));
        libusb_cancel_transfer(transfer);
        return -1;
    }

    while (nbytes_received == 0) {
        result = libusb_handle_events(ctx);
        if (result < 0) {
            /* Break out of this loop only on fatal error.*/
            if (result != LIBUSB_ERROR_BUSY &&
                result != LIBUSB_ERROR_TIMEOUT &&
                result != LIBUSB_ERROR_OVERFLOW &&
                result != LIBUSB_ERROR_INTERRUPTED) {
                fprintf(stderr, "Error %d receiving data via interrupt transfer: %s\n",
                    result, libusb_strerror(result));
                return result;
            }
        }
    }

    if (nbytes_received == LIBUSB_ERROR_TIMEOUT) {
        if (trace_flag > 0) {
            fprintf(stderr, "No response from HID device!\n");
        }
        goto again;
    }
    return nbytes_received;
}

//
// Send a request to the device.
// Store the reply into the rdata[] array.
// Terminate in case of errors.
//
void hid_send_recv(const unsigned char *data, unsigned nbytes, unsigned char *rdata, unsigned rlength)
{
    unsigned char buf[42];
    unsigned char reply[42];
    unsigned k;
    int reply_len;

    memset(buf, 0, sizeof(buf));
    buf[0] = 1;
    buf[1] = 0;
    buf[2] = nbytes;
    buf[3] = nbytes >> 8;
    if (nbytes > 0)
        memcpy(buf+4, data, nbytes);
    nbytes += 4;

    if (trace_flag > 0) {
        fprintf(stderr, "---Send");
        for (k=0; k<nbytes; ++k) {
            if (k != 0 && (k & 15) == 0)
                fprintf(stderr, "\n       ");
            fprintf(stderr, " %02x", buf[k]);
        }
        fprintf(stderr, "\n");
    }
    reply_len = write_read(buf, sizeof(buf), reply, sizeof(reply));
    if (reply_len < 0) {
        exit(-1);
    }
    if (reply_len != sizeof(reply)) {
        fprintf(stderr, "Short read: %d bytes instead of %d!\n",
            reply_len, (int)sizeof(reply));
        exit(-1);
    }
    if (trace_flag > 0) {
        fprintf(stderr, "---Recv");
        for (k=0; k<reply_len; ++k) {
            if (k != 0 && (k & 15) == 0)
                fprintf(stderr, "\n       ");
            fprintf(stderr, " %02x", reply[k]);
        }
        fprintf(stderr, "\n");
    }
    if (reply[0] != 3 || reply[1] != 0 || reply[3] != 0) {
        fprintf(stderr, "incorrect reply\n");
        exit(-1);
    }
    if (reply[2] != rlength) {
        fprintf(stderr, "incorrect reply length %d, expected %d\n",
            reply[2], rlength);
        exit(-1);
    }
    memcpy(rdata, reply+4, rlength);
}

//
// Connect to the specified device.
// Initiate the programming session.
//
int hid_init(int vid, int pid)
{
    int error = libusb_init(&ctx);
    if (error < 0) {
        fprintf(stderr, "libusb init failed: %d: %s\n",
            error, libusb_strerror(error));
        exit(-1);
    }

    dev = libusb_open_device_with_vid_pid(ctx, vid, pid);
    if (!dev) {
        if (trace_flag) {
            fprintf(stderr, "Cannot find USB device %04x:%04x\n",
                vid, pid);
        }
        libusb_exit(ctx);
        ctx = 0;
        return -1;
    }
    if (libusb_kernel_driver_active(dev, 0)) {
        libusb_detach_kernel_driver(dev, 0);
    }

    error = libusb_claim_interface(dev, HID_INTERFACE);
    if (error < 0) {
        fprintf(stderr, "Failed to claim USB interface: %d: %s\n",
            error, libusb_strerror(error));
        libusb_close(dev);
        libusb_exit(ctx);
        ctx = 0;
        exit(-1);
    }
    return 0;
}

void hid_close()
{
    if (!ctx)
        return;

    if (transfer) {
        libusb_free_transfer(transfer);
        transfer = 0;
    }
    libusb_release_interface(dev, HID_INTERFACE);
    libusb_close(dev);
    libusb_exit(ctx);
    ctx = 0;
}
