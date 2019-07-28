/*
    MIT License

    Copyright (C) 2018, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/err.h>
#include <linux/string.h>

#include "ssd1306.h"
#include "intf/ssd1306_interface.h"
#include "intf/i2c/ssd1306_i2c.h"
#include "font6x8.h"

#define DEVICE_CLASS_NAME "ssd1306_lcd"

static int bus = 1;
static int addr = 0x3C;
static struct i2c_client *s_client = NULL;
static struct ssd1306_data *s_data = NULL;
//static struct class *ssd1306_class = NULL;

module_param(bus, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(addr, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

MODULE_PARM_DESC(bus, " I2C Bus number, default 1");
MODULE_PARM_DESC(addr, " I2C device address, default 0x3C");

struct ssd1306_data {
	struct mutex lock;
	u8 index;
	/// According to https://www.kernel.org/doc/Documentation/i2c/dev-interface
	/// block buffers need not be longer than 32 bytes
	u8 data[24];
};


static void ssd1306_smbus_end(void);

static void ssd1306_smbus_start(void) {
	s_data->index = 0;
}

static void ssd1306_smbus_send(u8 data) {
	if (s_data->index >= sizeof(s_data->data)) {
		ssd1306_smbus_end();
		ssd1306_smbus_start();
		s_data->data[0] = 0x40;
		s_data->index++;
	}
	s_data->data[s_data->index] = data;
	s_data->index++;
}

static void ssd1306_smbus_send_bytes(const u8 *buffer, u16 size)
{
	while (size--) {
		ssd1306_smbus_send(*buffer);
		buffer++;
	}
}

static void ssd1306_smbus_end(void) {
	if (s_data->index) {
		i2c_smbus_write_i2c_block_data(s_client, s_data->data[0], s_data->index - 1, &s_data->data[1]);
		s_data->index=0;
	}
}

static void ssd1306_smbus_close(void) {
}


static ssize_t show_commands(struct device *dev,
				struct device_attribute *attr, char *buf)
{
//	struct ssd1306_data *data = i2c_get_clientdata(to_i2c_client(dev));
	return sprintf(buf, 	"clear\n"
				"pixel x,y\n"
	);
}

static ssize_t execute_command(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count)
{
	char cmd[16];
	s_client = to_i2c_client(dev);
	s_data = i2c_get_clientdata(s_client);

	mutex_lock(&s_data->lock);
	sscanf(buf, "%15s", cmd);
	if (!strcmp(cmd, "clear"))
		ssd1306_clearScreen();
	else if (!strcmp(cmd, "pixel")) {
		int x, y;
		sscanf(&buf[strlen(cmd) + 1], "%d,%d", &x, &y);
		ssd1306_putPixel(x,y);
	}
	mutex_unlock(&s_data->lock);
	return count;
}

static DEVICE_ATTR(command, S_IWUSR | S_IWGRP | S_IRUGO, show_commands, execute_command);

static struct attribute *ssd1306_attributes[] = {
	&dev_attr_command.attr,
	NULL
};

static const struct attribute_group ssd1306_attr_group = {
	.attrs = ssd1306_attributes,
};

static int ssd1306_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct ssd1306_data *data;
	int err;

	data = devm_kzalloc(&client->dev, sizeof(struct ssd1306_data),
			    GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	i2c_set_clientdata(client, data);
	mutex_init(&data->lock);
	s_data = data;
	ssd1306_startTransmission = ssd1306_smbus_start;
	ssd1306_endTransmission = ssd1306_smbus_end;
	ssd1306_sendByte = ssd1306_smbus_send;
	ssd1306_sendBytes = ssd1306_smbus_send_bytes;
	ssd1306_closeInterface = ssd1306_smbus_close;

	mutex_lock(&data->lock);
	s_client = client;
	ssd1306_128x64_init();
	ssd1306_fillScreen(0x00);
	ssd1306_setFixedFont(ssd1306xled_font6x8);
	ssd1306_printFixed (0,  8, "Line 1. text", 0); // STYLE_NORMAL
	mutex_unlock(&data->lock);


//	ssd1306_class = class_create(THIS_MODULE, DEVICE_CLASS_NAME);
	/* Register sysfs hooks */
	err = sysfs_create_group(&client->dev.kobj, &ssd1306_attr_group);
	if (err)
		return err;

	return 0;
}

static int ssd1306_remove(struct i2c_client *client)
{
//	struct ssd1306_data *data = i2c_get_clientdata(client);
	sysfs_remove_group(&client->dev.kobj, &ssd1306_attr_group);
	return 0;
}


static const struct i2c_device_id ssd1306_device_id[] = {
	{ "ssd1306", 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, ssd1306_device_id);

static struct i2c_driver ssd1306_driver = {
	.probe = ssd1306_probe,
	.remove = ssd1306_remove,
	.id_table = ssd1306_device_id,
	.driver = {
		.owner = THIS_MODULE,
		.name = "ssd1306"
	},
};


static int __init ssd1306_driver_init(void)
{
	int ret;
	struct i2c_adapter *adapter;
	struct i2c_board_info board_info = {
		.type = "ssd1306",
		.addr = addr,

	};
	adapter = i2c_get_adapter(bus);
	if (!adapter)
		return -EINVAL;
	s_client = i2c_new_device(adapter, &board_info);
	if (!s_client) {
		i2c_put_adapter(adapter);
		return -EINVAL;
	}
	ret = i2c_add_driver(&ssd1306_driver);
	if (ret) {
		i2c_put_adapter(adapter);
		return ret;
	}
	if (!i2c_check_functionality(adapter, I2C_FUNC_I2C)) {
		i2c_put_adapter(adapter);
		dev_err(&s_client->dev, "i2c bus error\n");
        	return -ENODEV;
	}
	i2c_put_adapter(adapter);
	dev_info(&s_client->dev, "registered ssd1306\n");
	return ret;
}

static void __exit ssd1306_driver_exit(void)
{
	if (s_client)
		i2c_unregister_device(s_client);
	i2c_del_driver(&ssd1306_driver);
}

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Aleksei Dynda <alexey.dynda@gmail.com>");
MODULE_DESCRIPTION("ssd1306 oled control driver");

module_init(ssd1306_driver_init);
module_exit(ssd1306_driver_exit);
