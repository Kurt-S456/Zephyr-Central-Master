#include "benchmark_shared.h"

#include <errno.h>

#include <zephyr/drivers/spi.h>
#include <zephyr/sys/printk.h>

#define CHILD_SPI_OPERATION (SPI_BASE_OPERATION | SPI_OP_MODE_SLAVE)

static const struct device *const spi_dev = DEVICE_DT_GET(SPI_DEV_NODE);
static const struct spi_config spi_cfg = {
	.operation = CHILD_SPI_OPERATION,
	.frequency = 1000000,
	.slave = 0,
};

int run_child(void)
{
	if (!device_is_ready(spi_dev)) {
		printk("child: SPI bus is not ready\n");
		return -ENODEV;
	}

	printk("child: waiting for worker transfers\n");

	while (1) {
		uint8_t rx_data[8] = {0};
		struct spi_buf rx_buf = {
			.buf = rx_data,
			.len = sizeof(rx_data),
		};
		struct spi_buf_set rx_set = {
			.buffers = &rx_buf,
			.count = 1,
		};

		int ret = spi_transceive(spi_dev, &spi_cfg, NULL, &rx_set);
		if (ret < 0) {
			printk("child: spi_transfer failed (%d)\n", ret);
			k_msleep(100);
			continue;
		}

		uint64_t worker_ts = decode_worker_timestamp(rx_data);
		printk("child: rx worker uptime=%llu ms\n",
		       (unsigned long long)worker_ts);
	}

	return 0;
}
