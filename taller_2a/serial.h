/*USB serial definitions*/
void serial_init(void);
void serial_task(void);
int16_t serial_read(void);
int8_t serial_write(uint8_t data);

