#include "benchmark_shared.h"

#include <errno.h>

#include <zephyr/drivers/spi.h>
#include <zephyr/sys/printk.h>

#define WORKER_SPI_OPERATION (SPI_BASE_OPERATION | SPI_OP_MODE_MASTER)

static const struct device *const spi_dev = DEVICE_DT_GET(SPI_DEV_NODE);
static const struct spi_config spi_cfg = {
	.operation = WORKER_SPI_OPERATION,
	.frequency = 1000000,
	.slave = 0,
};

int run_worker(void)
{
	if (!device_is_ready(spi_dev)) {
		printk("worker: SPI bus is not ready\n");
		return -ENODEV;
	}

	printk("worker: sending uptime timestamp every 1000 ms\n");

	while (1) {
		uint64_t worker_ts = (uint64_t)k_uptime_get();
		uint8_t tx_data[8];
		uint8_t rx_data[8] = {0};
		struct spi_buf tx_buf = {
			.buf = tx_data,
			.len = sizeof(tx_data),
		};
		struct spi_buf_set tx_set = {
			.buffers = &tx_buf,
			.count = 1,
		};
		struct spi_buf rx_buf = {
			.buf = rx_data,
			.len = sizeof(rx_data),
		};
		struct spi_buf_set rx_set = {
			.buffers = &rx_buf,
			.count = 1,
		};

		encode_worker_timestamp(worker_ts, tx_data);

		int ret = spi_transceive(spi_dev, &spi_cfg, &tx_set, &rx_set);
		if (ret < 0) {
			printk("worker: spi_transceive failed (%d)\n", ret);
			k_msleep(250);
			continue;
		}

		uint64_t child_ts = decode_worker_timestamp(rx_data);
		printk("worker: tx uptime=%llu ms, child reply=%llu ms\n",
		       (unsigned long long)worker_ts,
		       (unsigned long long)child_ts);
		k_msleep(1000);
	}

	return 0;
}
