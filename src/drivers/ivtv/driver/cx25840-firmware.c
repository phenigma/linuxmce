/*
    cx25840 firmware functions

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/i2c.h>
#include <linux/i2c-algo-bit.h>
#include <linux/mm.h>
#include <asm/uaccess.h>

#include "compat.h"

#include "cx25840.h"
#include "cx25840-firmware.h"
#include "cx25840-driver.h"
#include "cx25840-registers.h"

static inline void cx25840_i2c_set_delay(struct i2c_client *client, int delay)
{
	struct i2c_algo_bit_data *algod = client->adapter->algo_data;
	algod->udelay = delay;
}

static inline void start_fw_load(struct i2c_client *client)
{
	struct cx25840_info *info = i2c_get_clientdata(client);

	CX25840_SET_DL_ADDR_LB(0x00);   // 0x800
	CX25840_SET_DL_ADDR_HB(0x00);   // 0x801
	CX25840_SET_DL_MAP(0x03);       // 0x803
	CX25840_SET_DL_AUTO_INC(0x00);  // 0x803
	CX25840_SET_DL_ENABLE(0x01);    // 0x803
	CX25840_SET_AUTO_INC_DIS(0x01); // 0x000

	if (info->fastfw)
		cx25840_i2c_set_delay(client, 3);
}

static inline void end_fw_load(struct i2c_client *client)
{
	struct cx25840_info *info = i2c_get_clientdata(client);
	
	if (info->fastfw)
		cx25840_i2c_set_delay(client, 10);

	CX25840_SET_AUTO_INC_DIS(0x00); // 0x000
	CX25840_SET_DL_ENABLE(0x00);    // 0x803
}

static inline int check_fw_load(struct i2c_client *client, int size, const char *file)
{
	int s;

	s = cx25840_read_setting(client, DL_ADDR_HB) << 8;  // 0x801
	s += cx25840_read_setting(client, DL_ADDR_LB);       // 0x800
	if (size != s) {
		CX25840_ERR("firmware %s load failed\n", file);
		return -EINVAL;
	}
	CX25840_INFO("loaded %s firmware (%d bytes)\n", file, size);

	return 0;
}

static inline int fw_write(struct i2c_client *client, u8 *data, int size)
{
	struct cx25840_info *info = i2c_get_clientdata(client);
	
	if (i2c_master_send(client, data, size) < 0) {

		if (info->fastfw) {
			CX25840_ERR("333MHz i2c firmware load failed\n");
			info->fastfw = 0;
			cx25840_i2c_set_delay(client, 10);

			if (i2c_master_send(client, data, size) < 0) {
				CX25840_ERR("100MHz i2c firmware load failed\n");
				return -ENOSYS;
			}

		} else {
			CX25840_ERR("firmware load i2c failure\n");
			return -ENOSYS;
		}

	}

	return 0;
}

int cx25840_load_fw_hp(struct i2c_client *client)
{
	struct cx25840_info *info = i2c_get_clientdata(client);
	const struct firmware *fw = NULL;
	const char *file = info->firmware ? info->firmware : FWFILE;
	u8 buffer[4], *ptr;
	int size, retval;

	if (request_firmware(&fw, file, FWDEV(client)) != 0) {
		CX25840_ERR("unable to open firmware %s\n", file);
		return -EINVAL;
	}

	start_fw_load(client);

	buffer[0] = 0x08;
	buffer[1] = 0x02;
	buffer[2] = fw->data[0];
	buffer[3] = fw->data[1];
	retval = fw_write(client, buffer, 4);

	if (retval < 0) {
		release_firmware(fw);
		return retval;
	}

	size = fw->size - 2;
	ptr = fw->data;
	while (size > 0) {
		ptr[0] = 0x08;
		ptr[1] = 0x02;
		retval =
		    fw_write(client, ptr,
			     size > (FWSEND - 2) ? FWSEND : size + 2);

		if (retval < 0) {
			release_firmware(fw);
			return retval;
		}

		size -= FWSEND - 2;
		ptr += FWSEND - 2;
	}

	end_fw_load(client);

	size = fw->size;
	release_firmware(fw);

	return check_fw_load(client, size, file);
}

int cx25840_load_fw_nohp(struct i2c_client *client)
{
	struct cx25840_info *info = i2c_get_clientdata(client);
	const char *file = info->firmware ? info->firmware : FWFILE;
	loff_t file_offset = 0;
	mm_segment_t fs;
	kernel_filep filep;
	int size, tsize, retval;
	u8 *buffer;

	buffer = kmalloc(FWSEND, GFP_KERNEL);
	if (buffer == 0) {
		CX25840_ERR("firmware load failed (out of memory)\n");
		return -ENOMEM;
	}

	buffer[0] = 0x08;
	buffer[1] = 0x02;

	fs = get_fs();
	set_fs(get_ds());

	filep = kernel_file_open(file, 0, 0);
	if (kernel_file_is_err(filep)) {
		CX25840_ERR("unable to open firmware %s\n", file);
		kfree(buffer);
		set_fs(fs);
		return -EINVAL;
	}

	start_fw_load(client);

	tsize = 0;
	do {
		size =
		    kernel_file_read(filep, &buffer[2], FWSEND - 2,
				     &file_offset);
		if (size < 0) {
			CX25840_ERR("firmware %s read failed\n", file);
			kfree(buffer);
			kernel_file_close(filep);
			set_fs(fs);
			return -EINVAL;
		}

		tsize += size;

		if (size) {
			retval = fw_write(client, buffer, size + 2);

			if (retval < 0) {
				kfree(buffer);
				kernel_file_close(filep);
				set_fs(fs);
				return retval;
			}
		}

	} while (size == FWSEND - 2);

	end_fw_load(client);

	kfree(buffer);
	kernel_file_close(filep);
	set_fs(fs);

	return check_fw_load(client, tsize, file);
}
