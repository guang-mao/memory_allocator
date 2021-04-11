#ifndef __MEM_ALLOCATOR_H_
#define __MEM_ALLOCATOR_H_
#include "main.h"

#define MEMORY_POOL_SIZE											1024U
#define BLOCK_SIZE														32U
#define UART_FRAME_MAX_DATA_LEN               8U
#define UART_FRAME_MIN_DATA_LEN               2U

typedef struct TxQueueItem TxQueueItem;

typedef struct{
	uint8_t header;
	uint8_t type:7;
	uint8_t rw:1;
	uint8_t group:4;
	uint8_t id:4;
	uint8_t status:5;
	uint8_t length:3;
	uint8_t data[UART_FRAME_MAX_DATA_LEN];
	uint8_t crc_l;
	uint8_t crc_h;
} UART_LONG_FRAME;

typedef struct{
	uint8_t header;
	uint8_t type:7;
	uint8_t rw:1;
	uint8_t group:4;
	uint8_t id:4;
	uint8_t status:5;
	uint8_t length:3;
	uint8_t data[UART_FRAME_MIN_DATA_LEN];
	uint8_t crc_l;
	uint8_t crc_h;
} UART_SHORT_FRAME;

typedef union{
	UART_LONG_FRAME  long_frame;
	UART_SHORT_FRAME short_frame;
} MKS_UARTFrame;

typedef union PoolAllocatorBlock_u{
		uint8_t bytes[BLOCK_SIZE];
		union PoolAllocatorBlock_u *next;
} PoolAllocatorBlock;

typedef struct
{
    uint16_t capacity_blocks;               ///< Pool capacity in number of blocks
    uint16_t current_usage_blocks;          ///< Number of blocks that are currently allocated by the library
    uint16_t peak_usage_blocks;             ///< Maximum number of blocks used since initialization
} PoolAllocatorStatistics;

typedef struct{
		PoolAllocatorBlock *free_list;
		PoolAllocatorStatistics statistics;
} PoolAllocator;


void initPoolAllocator(PoolAllocator *allocator,
											 PoolAllocatorBlock* buf,
											 uint16_t block_page);

TxQueueItem* createTxItem(PoolAllocator *allocator);
uint16_t calcrc(uint8_t *frame, uint8_t length);
void* allocateBlock(PoolAllocator *allocator);
void freeBlock(PoolAllocator* allocator, void* p);

extern PoolAllocator instance;
extern uint8_t memory_pool[MEMORY_POOL_SIZE];
#endif
