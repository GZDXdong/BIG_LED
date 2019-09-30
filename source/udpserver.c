
#include "udpserver.h"

static void udp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

int udp_server_init(void)
{
   struct udp_pcb *upcb;
   err_t err;
   
   /* Create a new UDP control block  */
   upcb = udp_new();
   
   if (upcb != NULL)
   {
     /* Bind the upcb to the UDP_PORT port */
     /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
      err = udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);
      
      if(err == ERR_OK)
      {
        /* Set a receive callback for the upcb */
        udp_recv(upcb, udp_server_receive_callback, NULL);
        return ERR_OK;
      }
      else
        return ERR_VAL;
   }
   return ERR_VAL;
   
}


static void udp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  int recv_len = 0;
  
  /* Connect to the remote client */
//  udp_connect(upcb, addr, UDP_CLIENT_PORT);

  /* parse udpClient's data, added by postman 2019-08-07*/
  recv_len = p->len;
  memset(recv_buf, 0, sizeof(recv_buf));
  memcpy(recv_buf, p->payload, recv_len);

  /* Comment by postman in 2019-8-12 */
  if(sscanf(recv_buf, "ConcHr=%f;ConcRt=%f;AT=%f;RH=%f;BP=%f;\
            Longitude=%f;Latitude=%f;WindSpeed=%f;WindDir=%f;Noise=%f",\
            &ConcHr, &ConcRt, &AT, &RH, &BP, &Longitude, &Latitude, &WindSpeed, &WindDir, &Noise) == 10)
  {
    BP = BP/1000;// transfer to unit: KPa
    
    if (ConcHr >= 0.0 && ConcHr <= 1000.0)
      flag_ConcHr = 1;
    else
      flag_ConcHr = 0;
    
    if (ConcRt >= 0.0 && ConcRt <= 1000.0)
      flag_ConcRt = 1;
    else
      flag_ConcRt = 0;
    
    if (AT >= -40.0 && AT <= 120.0)
      flag_AT = 1;
    else
      flag_AT = 0;
    
    if (RH >= 0.0 && RH <= 100.0 )
      flag_RH = 1;
    else
      flag_RH = 0;
    
    if (BP >= 0.0 && BP <= 1000.0)
      flag_BP = 1;
    else
      flag_BP = 0;
    
    if (Longitude <= 180)
      flag_Longitude = 1;
    else
      flag_Longitude = 0;
    
    if (Latitude <= 90.0)
      flag_Latitude = 1;
    else
      flag_Latitude = 0;
    
    if (WindSpeed >= 0.0 && WindSpeed <= 70.0)
      flag_WindSpeed = 1;
    else
      flag_WindSpeed = 0;
    
    if (WindDir >= 0.0 && WindDir <= 360.0)
      flag_WindDir = 1;
    else
      flag_WindDir = 0;
    
    if (Noise >= 0.0 && Noise <= 130.0)
      flag_Noise = 1;
    else
      flag_Noise = 0;
    
    flag_display = 1;
    
  }
  else
  {
    flag_display = 0;
  }
  
  /* Tell the client that we have accepted it */
//  udp_send(upcb, p);
  
  /* free the UDP connection, so we can accept new clients */
//  udp_disconnect(upcb);
	
  /* Free the p buffer */
  pbuf_free(p);
   
}

