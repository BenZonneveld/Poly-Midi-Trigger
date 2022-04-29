#ifdef __cplusplus
extern "C"
{
#endif
	void tud_print(char* msg);
	void cdc_task(void* params);
	void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts);
	void tud_cdc_rx_cb(uint8_t itf);
#ifdef __cplusplus
}
#endif