#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define sla_w 0xD0
#define sla_r 0xD1
void led_config(void)
{
  DDRB = DDRB| 0x20;
  PORTB = PORTB & 0xDF;
}

void led_on(void)
{
  PORTB = PORTB|0x20;
}

void led_off(void)
{
  PORTB = PORTB&0xDF;
}

void port_init(void)
{
  led_config();
}

void twi_init(void)
{
  TWBR = 0x03;
  TWSR = TWSR|0x01;
  TWCR = (1<<TWEN);
}

int twi_start(void)
{
  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));
  if((TWSR & 0xF8) == 0x08)
    return 1;
  else
    return 0;
}

int twi_repeatedstart(void)
{
  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));
  if((TWSR & 0xF8) == 0x10)
    return 1;
  else
    return 0;
}

void twi_stop(void)
{
  TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
  while(!(TWCR & (1<<TWSTO)));
}

int twi_writeaddr(char addr)
{
  TWDR = addr;
  TWCR = (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));
  if((TWSR & 0xF8) == 0x18)
    return 1;
  else if((TWSR & 0xF8) == 0x20)
    return 0;
  else 
    return -1;
}

int twi_writedata(char data)
{
  TWDR = data;
  TWCR = (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));
  if((TWSR & 0xF8) == 0x28)
    return 1;
  else if((TWSR & 0xF8) == 0x30)
    return 0;
  else 
    return -1;
}

int twi_readaddr(char addr)
{
  TWDR = addr;
  TWCR = (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));
    if((TWSR & 0xF8) == 0x40)
      return 1;
    else if((TWSR & 0xF8) == 0x48)
      return 0;
  else 
    return -1;
}

int twi_readack()
{
  //int data;
  TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));
  //data = TWDR;
  return TWDR;
}
int twi_readnack()
{
  //int data;
  TWCR = (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));
  //data = TWDR;
  return TWDR;
  /*if((TWSR & 0xF8) == 0x50)
   return 1;
  else if((TWSR & 0xF8) == 0x58)
    return 0;
  else 
    return -1;*/
}

void init_devices (void)
{
 cli(); 
 port_init();
 twi_init();
 sei(); 
}

void initialise(void)
{
  _delay_ms(150);
  
  twi_start();
  twi_writeaddr(sla_w);
  twi_writeaddr(0x68);
  twi_writedata(0x00);
  twi_stop();
  
  twi_start();
  twi_writeaddr(sla_w);
  twi_writeaddr(0x6B);
  twi_writedata(0x01);
  twi_stop();
  
  twi_start();
  twi_writeaddr(sla_w);
  twi_writeaddr(0x1B);
  twi_writedata(0x10);
  twi_stop();

  twi_start();
  twi_writeaddr(sla_w);
  twi_writeaddr(0x1C);
  twi_writedata(0x10);
  twi_stop();

}
void setup() {
  // put your setup code here, to run once:
  init_devices();
  initialise();
  Serial.begin(9600);
  
}






void loop() {
 int temp,accX,accY,accZ,gyroX,gyroY,gyroZ;
 float t,ax,ay,az,gx,gy,gz;
 twi_start();
 twi_writeaddr(sla_w);
 twi_writeaddr(0x3B);
 twi_repeatedstart();
 twi_readaddr(sla_r);
 accX = ((int)twi_readack()<<8 | (int)twi_readack());
 accY = ((int)twi_readack()<<8 | (int)twi_readack());
 accZ = ((int)twi_readack()<<8 | (int)twi_readack());
 temp = ((int)twi_readack()<<8 | (int)twi_readack());
 gyroX = ((int)twi_readack()<<8 | (int)twi_readack());
 gyroY = ((int)twi_readack()<<8 | (int)twi_readack());
 gyroZ = ((int)twi_readack()<<8 | (int)twi_readnack());
 twi_stop();
 t = (temp/340.00)+36.53;
 ax = accX/4096.0;
 ay = accY/4096.0;
 az = accZ/4096.0;
 gx = gyroX/32.8;
 gy = gyroY/32.8;
 gz = gyroZ/32.8;
 /*Serial.print(ax);
 Serial.print('\t');
 Serial.print(ay);
 Serial.print('\t');
 Serial.print(az);
 Serial.print('\t');
 Serial.print(gx);
 Serial.print('\t');
 Serial.print(gy);*/
 Serial.print(t);
 Serial.println();
}
 
