/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Best-effort single-hop unicast example
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include "sys/node-id.h" // Include this library in order to be able to set node's ID.
#include "node-environment_CM5000.h" // Declares the struct environment
#include "/home/sink/Desktop/contiki-3.0/dev/sht11/sht11-sensor.h"
#include "/home/sink/Desktop/contiki-3.0/platform/sky/dev/light-sensor.h"
#include "/home/sink/Desktop/contiki-3.0/dev/cc2420/cc2420.h" // Include the CC2420 library
/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
PROCESS(read_temperature_light, "Reads the Sensors Information"); // Process for reading the temperature and light values
AUTOSTART_PROCESSES(&example_unicast_process, &read_temperature_light);
/*---------------------------------------------------------------------------*/
struct environment envir; // Struct used to store the temperature and light values  
int sequence = 0; // A sequence number that enumerates the data from 0 and increases in 1 each time.
/*---------------------------------------------------------------------------*/
static int
get_light(void) //Get the light value from sensor
{
  return 10 * light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC) / 7;
}
/*---------------------------------------------------------------------------*/
static int
get_temp(void) //Get the temperature value from sensor
{
  return ((sht11_sensor.value(SHT11_SENSOR_TEMP) / 10) - 396) / 10;
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  printf("unicast message received from %d.%d\n",
	 from->u8[0], from->u8[1]);
}
/*---------------------------------------------------------------------------*/
static void
sent_uc(struct unicast_conn *c, int status, int num_tx)
{
  const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
  if(linkaddr_cmp(dest, &linkaddr_null)) {
    return;
  }
  printf("unicast message sent to %d.%d: status %d num_tx %d\n",
    dest->u8[0], dest->u8[1], status, num_tx);
}
/*---------------------------------------------------------------------------*/
static const struct unicast_callbacks unicast_callbacks = {recv_uc, sent_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(read_temperature_light, ev, data)  // Process for reading the temperature and light values
{

  static struct etimer et; // Struct used for the timer
  int temporal; //Temporal Variable

  PROCESS_BEGIN();  // Says where the process starts 


  while(1){
  
  etimer_set(&et, CLOCK_SECOND * 5); // Configure timer to expire in 5 seconds
 
  SENSORS_ACTIVATE(light_sensor); // Activate light sensor
  SENSORS_ACTIVATE(sht11_sensor); // Activate temperature sensor
 
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et)); // Wait until timer expires 
 
  /*printf("Data\t"); // Print the string "Data"
  sequence++;
  printf("%d\t", sequence);  // Print the sequence number
  envir.sequence = sequence; // To save the sequence in the struct envir
  temporal = get_temp();
  printf("%d\t", temporal ); // Print the temperature value
  envir.temp = temporal; // To save the temperature in the struct envir
  temporal = get_light();
  printf("%d\n", temporal );  // Print the light value
  envir.light = temporal; // To save the light in the struct envir*/
  

  printf("Data/"); // Print the string "Data"
  sequence++;
  printf("%d/", sequence);  // Print the sequence number
  envir.sequence = sequence; // To save the sequence in the struct envir
  temporal = get_temp();
  printf("%d/", temporal ); // Print the temperature value
  envir.temp = temporal; // To save the temperature in the struct envir
  temporal = get_light();
  printf("%d\n\r", temporal );  // Print the light value
  envir.light = temporal; // To save the light in the struct envir
  

  process_post(&example_unicast_process, PROCESS_EVENT_CONTINUE , &(envir) ); // This function posts an asynchronous event to the process example_unicast_process with the information of the structure called envir
 
  etimer_reset(&et); // Reset timer
 
  SENSORS_DEACTIVATE(light_sensor); // Deactivate light sensor
  SENSORS_DEACTIVATE(sht11_sensor);  // Deactivate temperature sensor
  
  }
  
  PROCESS_END();//Says where the process ends

}


/*---------------------------------------------------------------------------*/

PROCESS_THREAD(example_unicast_process, ev, data)
{

  unsigned short id = 16; // This is the ID which will be set in your sky mote
  struct environment *envirRX =  data; //Saves the information that comes from the other process (read_temperature_light) into a structure pointer called *envirRX

  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();

  cc2420_set_txpower(31); //Max value. Set the output power of the node
  unicast_open(&uc, 146, &unicast_callbacks);
  node_id_burn(id); // Call this function to burn the defined id
  timesynch_set_authority_level(id);


  while(1) {
    //static struct etimer et;
    linkaddr_t addr;
    
    //etimer_set(&et, CLOCK_SECOND);
    
    //PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    PROCESS_WAIT_EVENT(); //Wait for an event to be posted to the process. 

    printf("Data/"); // Print the string "Data"
    printf("%d/", envirRX->sequence );  // Print the sequence number
    printf("%d/", envirRX->temp ); // Print the temperature value
    printf("%d\n\r", envirRX->light );  // Print the light value

    //packetbuf_copyfrom("Hello", 5);
    addr.u8[0] = 1;
    addr.u8[1] = 0;

#if TIMESYNCH_CONF_ENABLED
      envirRX->timestamp = timesynch_time();
#else
      envirRX->timestamp = 0;
#endif
      //packetbuf_copyfrom(  envirRX , sizeof(  (*envirRX)  ) ); 

    packetbuf_copyfrom("Hello", 5);
    if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {

      unicast_send(&uc, &addr);
    }

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
