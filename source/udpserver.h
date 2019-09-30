

#include "udp.h"
#include "pbuf.h"
#include "ethernetif.h"
#include "string.h"
#include "stdio.h"

/*Static IP ADDRESS*/
#define IP_ADDR0   (uint8_t) 192
#define IP_ADDR1   (uint8_t) 168
#define IP_ADDR2   (uint8_t) 1
#define IP_ADDR3   (uint8_t) 100
   
/*NETMASK*/
#define NETMASK_ADDR0   (uint8_t) 255
#define NETMASK_ADDR1   (uint8_t) 255
#define NETMASK_ADDR2   (uint8_t) 255
#define NETMASK_ADDR3   (uint8_t) 0

/*Gateway Address*/
#define GW_ADDR0   (uint8_t) 192
#define GW_ADDR1   (uint8_t) 168
#define GW_ADDR2   (uint8_t) 1
#define GW_ADDR3   (uint8_t) 1 
   
/* add by postman in 20190702 */   
#define DEST_IP_ADDR0   (uint8_t) 192
#define DEST_IP_ADDR1   (uint8_t) 168
#define DEST_IP_ADDR2   (uint8_t) 1
#define DEST_IP_ADDR3   (uint8_t) 98

#define UDP_SERVER_PORT    (uint16_t) 49153   /* define the UDP local connection port */

#define BUF_SIZE 1024

extern uint32_t message_count;
extern struct udp_pcb *upcb;

/* udp接收的数据 */
extern float ConcHr;
extern float ConcRt;
extern float AT;
extern float RH;
extern float BP;
extern float Longitude;
extern float Latitude;
extern float WindSpeed;
extern float WindDir;
extern float Noise;
extern char recv_buf[BUF_SIZE];
extern char flag_display;
extern char flag_ConcHr;
extern char flag_ConcRt;
extern char flag_AT;
extern char flag_RH;
extern char flag_BP;
extern char flag_Longitude;
extern char flag_Latitude;
extern char flag_WindSpeed;
extern char flag_WindDir;
extern char flag_Noise;

//void udp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

int udp_server_init(void);
