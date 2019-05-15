
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include "esp_partition.h"
#include "esp_log.h"

esp_partition_t *partition = NULL;

int flash_buf_read(size_t addr, const void *data, size_t size)
{
    esp_err_t ret = 0;
    assert(partition != NULL);
    ret = esp_partition_read(partition, addr, data, size);
    return ret ? -1 : size;
}

int flash_buf_write(size_t addr, const void *data, size_t size)
{
    esp_err_t ret = 0;
    assert(partition != NULL);
    esp_partition_write(partition, addr, data, size);
    return ret ? -1 : size;
}

int flash_buf_erase()
{
    assert(partition != NULL);
    esp_partition_erase_range(partition, 0, partition->size);
    return 0;
}

int flash_buf_get_size()
{
    assert(partition != NULL);
    return partition->size;
}

int flash_buf_deinit()
{
    // TODO
    return 0;
}

int flash_buf_init()
{
    partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    assert(partition != NULL);
    return 0;
}