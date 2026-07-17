#ifndef BENCHMARK_SHARED_H_
#define BENCHMARK_SHARED_H_

#include <stdint.h>

#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>

#define SPI_DEV_NODE DT_NODELABEL(spi1)

#if !DT_NODE_HAS_STATUS(SPI_DEV_NODE, okay)
#error "spi1 must be enabled for this sample"
#endif

#define SPI_BASE_OPERATION (SPI_WORD_SET(8) | SPI_TRANSFER_MSB)

static inline void encode_u64_be(uint64_t value, uint8_t out[8])
{
	for (int i = 0; i < 8; i++) {
		out[i] = (uint8_t)((value >> (56 - (8 * i))) & 0xFFU);
	}
}

static inline void encode_worker_timestamp(uint64_t worker_ts, uint8_t tx[8])
{
	for (int i = 0; i < 8; i++) {
		/* TX_i = ((T_worker >> (56 - 8i)) & 0xFF) */
		tx[i] = (uint8_t)((worker_ts >> (56 - (8 * i))) & 0xFFU);
	}
}

static inline uint64_t decode_worker_timestamp(const uint8_t rx_data[8])
{
	uint64_t worker_ts = 0;

	for (int i = 0; i < 8; i++) {
		worker_ts = (worker_ts << 8) | rx_data[i];
	}

	return worker_ts;
}

int run_worker(void);
int run_child(void);

#endif /* BENCHMARK_SHARED_H_ */
