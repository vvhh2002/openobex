/** \file apps/obex_find.c
 * Find connectable OBEX devices (IrDA, USB).
 * OpenOBEX test applications and sample code.
 *
 * Copyright (c) 2010 Hendrik Sattler
 *
 * OpenOBEX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with OpenOBEX. If not, see <http://www.gnu.org/>.
 */

#include <openobex/obex.h>
#include <openobex/obex_const.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

static void obex_event_cb(obex_t *handle, obex_object_t *obj, int mode,
			  int event, int obex_cmd, int obex_rsp)
{
}

static void usb_print(obex_interface_t *intf)
{
	printf("\tManufacturer: %s\n", intf->usb.manufacturer);
	printf("\tProduct: %s\n", intf->usb.product);
	printf("\tSerial: %s\n", intf->usb.serial);
	printf("\tDescription: %s\n", intf->usb.control_interface);
}

static void irda_print(obex_interface_t *intf)
{
	char* charset = "";

	if (intf->irda.local)
		printf("\tLocal address: %08x\n", intf->irda.local);
	printf("\tRemote address: %08x\n", intf->irda.remote);
	if (intf->irda.charset == 0xFF)
		charset = strdup("Unicode");
	else if (intf->irda.charset == 0x00)
		charset = strdup("ASCII");
	else if (intf->irda.charset <= 9) {
		charset = strdup("ISO-8859-1");
		charset[9] = '0'+intf->irda.charset;
	}
	printf("\tDescription character set: %s\n", charset);
	if (strlen(charset))
		free(charset);
	if (intf->irda.charset == 0x00)
		printf("\tDescription: %s\n", intf->irda.info);
}

static void find_devices(int trans, int flags)
{
	obex_t *handle;
	int found;
	int i = 0;
	const char *descr = NULL;
	void (*trans_print)(obex_interface_t *);

	switch (trans) {
	case OBEX_TRANS_IRDA:
		descr = "irda";
		trans_print = irda_print;
		break;

	case OBEX_TRANS_USB:
		descr = "usb";
		trans_print = usb_print;
		break;

	default:
		return;
	}
  
	handle = OBEX_Init(trans, obex_event_cb, flags);
	found = OBEX_EnumerateInterfaces(handle);
	printf("Found %d %s devices.\n", found, descr);
	for (; i < found; ++i) {
		obex_interface_t *intf = OBEX_GetInterfaceByIndex(handle, i);
		printf("Device %d:\n", i);
		trans_print(intf);
	}
	OBEX_Cleanup(handle);
}

int main (int argc, char **argv)
{
	int t = 1;
	char *default_argv[] = { argv[0], "irda", "usb", NULL };

	if (argc < 2)
		argv = default_argv;

	for (; argv[t] != NULL; ++t) {
		if (0 == strcasecmp(argv[t], "irda"))
			find_devices(OBEX_TRANS_IRDA, OBEX_FL_FILTERHINT);
		else if (0 == strcasecmp(argv[t], "usb"))
			find_devices(OBEX_TRANS_USB, 0);
	}
	return 0;
}