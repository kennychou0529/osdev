/* vim: tabstop=4 shiftwidth=4 noexpandtab
 *
 * ToAruOS PCI Initialization
 */

#include <system.h>
#include <ata.h>

ide_channel_regs_t ide_channels[2];
ide_device_t ide_devices[4];
uint8_t ide_buf[2048] = {0};
uint8_t ide_irq_invoked = 0;
uint8_t atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void ide_detect() {
	
}

void ide_read_sector(uint16_t bus, uint8_t slave, uint32_t lba, uint8_t * buf) {
	outportb(bus + ATA_REG_FEATURES, 0x00);
	outportb(bus + ATA_REG_SECCOUNT0, 1);
	outportb(bus + ATA_REG_HDDEVSEL,  0xe0 | slave << 4 | 
								 (lba & 0x0f000000) >> 24);
	outportb(bus + ATA_REG_LBA0, (lba & 0x000000ff) >>  0);
	outportb(bus + ATA_REG_LBA1, (lba & 0x0000ff00) >>  8);
	outportb(bus + ATA_REG_LBA2, (lba & 0x00ff0000) >> 16);
	outportb(bus + ATA_REG_COMMAND, ATA_CMD_READ_PIO);
	uint8_t status = 0;
	while ((status = inportb(bus + 0x07)) & 0x80);
	int size = 256;
	inportsm(bus,buf,size);
}

void ide_write_sector(uint16_t bus, uint8_t slave, uint32_t lba, uint8_t * buf) {
	outportb(bus + ATA_REG_FEATURES, 0x00);
	outportb(bus + ATA_REG_SECCOUNT0, 0x01);
	outportb(bus + ATA_REG_HDDEVSEL,  0xe0 | slave << 4 | 
								 (lba & 0x0f000000) >> 24);
	outportb(bus + ATA_REG_LBA0, (lba & 0x000000ff) >>  0);
	outportb(bus + ATA_REG_LBA1, (lba & 0x0000ff00) >>  8);
	outportb(bus + ATA_REG_LBA2, (lba & 0x00ff0000) >> 16);
	outportb(bus + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);
	uint8_t status = 0;
	while ((status = inportb(bus + 0x07)) & 0x80);
	int size = 256;
	outportsm(bus,buf,size);
	outportb(bus + 0x07, ATA_CMD_CACHE_FLUSH);
}
