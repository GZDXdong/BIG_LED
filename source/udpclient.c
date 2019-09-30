/**
  ******************************************************************************
  * @file    udpclient.c 
  * @author  Postman
  * @brief   udp_send & udp_recv
  *          
  ******************************************************************************
  */

#include "udpclient.h"

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */

/* 数据接收异常，commented by postman 2019-07-03 */   
/* 数据接收正常，commented by postman 2019-07-04 */
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{

  /* 
  通过 sscanf() 获取格式化数据
  PM2.5---%d、PM10---%d、TSP---%d、
  温度---%f、湿度---%f、气压---%f、
  风速---%f、风向---%d、噪声---%d
  */ 

  int recv_len = 0;
  recv_len = p->len;
  memset(recv_buf, 0, sizeof(recv_buf));
  memcpy(recv_buf, p->payload, recv_len);
  
//  if(sscanf(recv_buf, "PM2.5 = %d, PM10 = %d, TSP = %d, TEMP = %f, HUMID = %f, KPA = %f, SPEED = %f, DIR = %d, NOISE = %d", \
//    &PM_25, &PM_10, &TSP, &TEMP, &HUMID, &KPA, &wind_spd, &wind_dir, &noise) > 0)
//    message_count++;

  /* Comment by postman in 2019-8-12 */
  if(sscanf(recv_buf, "ConcHr=%f;ConcRt=%f;AT=%f;RH=%f;\
            BP=%f;Longitude=%f;Latitude=%f;WindSpeed=%f;WindDir=%f;Noise=%f", \
            &ConcHr, &ConcRt, &AT, &RH, &BP, &Longitude, &Latitude, &WindSpeed, &WindDir, &Noise) == 10)
  {
    message_count++;
  }

  pbuf_free(p);
  
}

int udp_connect_to_server(void)
{
  ip_addr_t DestIPaddr;
  err_t err;
  
  /*assign destination IP address */
  IP4_ADDR(&DestIPaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3);
  
  /* Create a new UDP control block */
  upcb = udp_new();
  
  if (upcb!=NULL)
  {
    
    /* configure destination IP address and port */
    err= udp_connect(upcb, &DestIPaddr, UDP_SERVER_PORT);
    
    if (err == ERR_OK)
    {
      /* Set a receive callback for the upcb */
      udp_recv(upcb, udp_receive_callback, NULL);
      return ERR_OK;
    }
  }
  
  return ERR_VAL;

}

void udp_client_send(const char *send_buf)
{
  int str_len = 0;
  struct pbuf *psend;
  err_t err = 0;

  str_len = strlen(send_buf);
  psend = pbuf_alloc(PBUF_TRANSPORT, str_len, PBUF_RAM);// Allocate a PBUF_RAM to send the data
  
  if (psend == NULL)
    return;
  
  memcpy(psend->payload, send_buf, str_len);// copy data to pbuf
  
  err = udp_send(upcb, psend);// send udp data
  
  if (err != ERR_OK)
  {
    pbuf_free(psend);
    return;
  }
  pbuf_free(psend);// free pbuf
}




