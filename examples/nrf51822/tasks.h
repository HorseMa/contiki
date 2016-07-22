#ifndef __USER_PROCESS_H__
#define __USER_PROCESS_H__

extern process_event_t ev_433_tx_over;
extern process_event_t ev_433_rx_over;
extern process_event_t ev_checkw5500;
extern process_event_t ev_radio_rcv;
extern process_event_t ev_2_4g_rcv;
extern process_event_t ev_data_report_start;
extern process_event_t ev_data_report_end;

PROCESS_NAME(led_process);
PROCESS_NAME(read_gpio_process);
PROCESS_NAME(uartRecv_process);
PROCESS_NAME(si4463_center_process);
PROCESS_NAME(si4463_enddev_process);
PROCESS_NAME(ethernet_process);
PROCESS_NAME(data_report_process);

#endif