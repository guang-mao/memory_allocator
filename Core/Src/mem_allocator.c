#include "main.h"
#include "mem_allocator.h"
uint8_t memory_pool[MEMORY_POOL_SIZE];
PoolAllocator instance;

struct TxQueueItem{
	TxQueueItem *next;
	MKS_UARTFrame frame;
};

void initPoolAllocator(PoolAllocator *allocator,
											 PoolAllocatorBlock* buf,
											 uint16_t block_page){
	uint16_t current_index = 0;
	PoolAllocatorBlock **current_block = &allocator->free_list;
	while(current_index < block_page){
		*current_block = &buf[current_index];
		current_block = &((*current_block)->next);
		current_index++;
	}
	*current_block = NULL;
	allocator->statistics.capacity_blocks = block_page;
	allocator->statistics.current_usage_blocks = 0;
//	ins->statistics.peak_usage_blocks = 0;
}

int16_t enqueueTxFrames(PoolAllocator *ins,
												MKS_UARTFrame *tx_frame,
												uint8_t payload_len){
	uint16_t crc; 											
	assert_param(ins != NULL);
	assert_param(tx_frame != NULL);
	TxQueueItem *queue_item = createTxItem(ins);
	if( payload_len == 8){
		queue_item->frame.long_frame = tx_frame->long_frame;
		queue_item->frame.long_frame.header  = 0xBC;
		crc = calcrc( (uint8_t *) &queue_item->frame.long_frame, 14);
		queue_item->frame.long_frame.crc_l = (crc & 0x00FF);
		queue_item->frame.long_frame.crc_h = (crc & 0xFF00) >> 8;
	}else{ 
		queue_item->frame.short_frame = tx_frame->short_frame;
		queue_item->frame.short_frame.header = 0xBC;
		crc = calcrc( (uint8_t *) &queue_item->frame.short_frame, 6);
		queue_item->frame.short_frame.crc_l = (crc & 0x00FF);
		queue_item->frame.short_frame.crc_h = (crc & 0xFF00) >> 8;
	}
	return 0;						
}

TxQueueItem* createTxItem(PoolAllocator *allocator){
	TxQueueItem *item = (TxQueueItem *) allocateBlock(allocator);
	if(item == NULL){
		return NULL;
	}
	memset(item, 0, sizeof(*item));
	return item;
}

uint16_t calcrc(uint8_t *frame, uint8_t length){
	uint16_t crc = 0;
	int j;
	for(j = 0; j < length; j++){
		crc += *frame++;
	}
	crc &= (0xffff);

	return crc;
}
											 
void* allocateBlock(PoolAllocator *allocator){
	
	if(allocator->free_list == NULL){
		return NULL;
	}
	void *result = allocator->free_list;
	allocator->free_list = allocator->free_list->next;
	
	allocator->statistics.current_usage_blocks++;
	
	return result;
}

void freeBlock(PoolAllocator* allocator, void* p){
	PoolAllocatorBlock* block = (PoolAllocatorBlock *) p;
	
	block->next = allocator->free_list;
	allocator->free_list = block;
	
	assert_param(allocator->statistics.current_usage_blocks > 0);
	allocator->statistics.current_usage_blocks--;
}


