/* 
 *  Rainbow LED Button ( GPIO ver )
 *  Author       : sana2dang ( fly9p ) - sana2dang@naver.com / sana2dang@gmail.com
 *  Creation Date: 2017 - 04 - 01
 *  Cafe         : http://cafe.naver.com/raspigamer
 *  Thanks to    : zzeromin, smyani, GreatKStar, KimPanda, StarNDevil, angel
 * 
 * - Reference -
 * wiringPi
 * http://wiringpi.com/the-gpio-utility/
 * apt-get install wiringPi
 * 
 * jstest
 * https://github.com/flosse/linuxconsole/blob/master/utils/jstest.c
  *
 * - complie -
 * sudo gcc RainbowLedBtn.c -o RainbowLedBtn -lwiringPi -lpthread
 * 
 */
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <pthread.h>

#include <linux/input.h>
#include <linux/joystick.h>

#define NAME_LENGTH 128

#define pin1r  21	// GPIO 20 A(G)
#define pin1g  20	// GPIO 21 A(B)
#define pin1b  26	// GPIO 26 A(R)
#define pin2  16	// GPIO 16 B
#define pin3  19	// GPIO 19 X
#define pin4  13	// GPIO 13 Y
#define pin5  6		// GPIO 6  L
#define pin6  12	// GPIO 12 R

#define AbtnShotPressTime 700	
#define AbtnLongPressTime 1500
#define stayTime 10000

int RGBbtnPress	= 0;	// RGBbtnPress on/off flag
int chargeShot	= 0;	// chargeShot cnt
long sTime	= 0;	// RGBbtnPress Start Time
int chargeCnt 	= 0;	// Charge Count
int k = 0;		// Led Cycle Count


long sStopTime = 0;	// stop time
int rotationFlag = 0;	// rotation
int rotationCnt = 0;
int mode = 0;


void funAllLightOn(int mode)
{
	if( mode == 0 )		// ALL LED ON
	{
		digitalWrite(pin1r,1);
		digitalWrite(pin1g,1);
		digitalWrite(pin1b,1);
		digitalWrite(pin2,1);
		digitalWrite(pin3,1);
		digitalWrite(pin4,1);
		digitalWrite(pin5,1);
		digitalWrite(pin6,1);
	}
	if( mode == 1 )		// ALL LED ON without A btn
	{
		digitalWrite(pin2,1);
		digitalWrite(pin3,1);
		digitalWrite(pin4,1);
		digitalWrite(pin5,1);
		digitalWrite(pin6,1);
	}
}

void funAllLightOff(int mode)
{
	if( mode == 0 )		// ALL LED OFF
	{
		digitalWrite(pin1r,0);
		digitalWrite(pin1g,0);
		digitalWrite(pin1b,0);
		digitalWrite(pin2,0);
		digitalWrite(pin3,0);
		digitalWrite(pin4,0);
		digitalWrite(pin5,0);
		digitalWrite(pin6,0);
	}
	if( mode == 1 )		// ALL LED OFF without A btn
	{
		digitalWrite(pin2,0);
		digitalWrite(pin3,0);
		digitalWrite(pin4,0);
		digitalWrite(pin5,0);
		digitalWrite(pin6,0);
	}

}


void funRainbowLed()
{	
	if( rotationFlag == 1)	// stay button led
	{
		switch( rotationCnt )
		{
			case 0:
				digitalWrite(pin1r,1);
				digitalWrite(pin1g,1);
				digitalWrite(pin1b,1);
				rotationCnt ++;
			break;
			case 1: 
				digitalWrite(pin2,1);
				rotationCnt ++;
			break;
			case 2: 
				digitalWrite(pin6,1);
				rotationCnt ++;
			break;
			case 3: 
				digitalWrite(pin5,1);
				rotationCnt ++;
			break;
			case 4: 
				digitalWrite(pin4,1);
				rotationCnt ++;
			break;
			case 5: 
				digitalWrite(pin3,1);
				rotationCnt = 0;
			break;	
			default: 
				funAllLightOff(0);	
		}
		delay(500);		// 0.5sec 
		funAllLightOff(0);		
	
	}
	else
	{
		rotationCnt = 0;	
	}

	// A Btn Event
	if( RGBbtnPress == 1 && chargeShot == 0 )
	{
		switch( chargeCnt )
		{
			case 0: 
				digitalWrite(pin1r,1);
				digitalWrite(pin1g,1);
				digitalWrite(pin1b,1);				
			break;
			case 1:
				digitalWrite(pin1r,0);
				digitalWrite(pin1g,1);
				digitalWrite(pin1b,1);
			break;
			case 2: 
				digitalWrite(pin1r,0);
				digitalWrite(pin1g,1);
				digitalWrite(pin1b,0);
			break;			
			case 3: 
				digitalWrite(pin1r,1);
				digitalWrite(pin1g,1);
				digitalWrite(pin1b,0);			
			break;
			case 4: 
				digitalWrite(pin1r,1);
				digitalWrite(pin1g,0);
				digitalWrite(pin1b,0);
			break;
			case 5: 
				digitalWrite(pin1r,1);
				digitalWrite(pin1g,0);
				digitalWrite(pin1b,1);				
			break;
			case 6: 
				digitalWrite(pin1r,1);
				digitalWrite(pin1g,1);
				digitalWrite(pin1b,1);				
			break;
			default: 
				digitalWrite(pin1r,0);
				digitalWrite(pin1g,0);
				digitalWrite(pin1b,0);				
				chargeCnt = 0;
			break;
		}
		delay(50);		

		if( ( millis() - sTime ) < AbtnShotPressTime )		// RGBbtnPress keep check1
		{					
			chargeCnt = 0;
		}
		else
		{
			
			if( ( millis() - sTime )  > AbtnLongPressTime )	// RGBbtnPress keep check2
			{				
				chargeShot = 1;				
				digitalWrite(pin1r,0);	// RGB Led Off
				digitalWrite(pin1g,0);	// RGB Led Off
				digitalWrite(pin1b,0);	// RGB Led Off
			}
			else
			{
				chargeCnt++;
			}
		}
	}
	if( RGBbtnPress == 0 )
	{	
		if( chargeShot == 1 )
		{
			for(k=0; k<2; k++)
			{
				digitalWrite(pin1r,1);
				digitalWrite(pin1g,0);
				digitalWrite(pin1b,0);
				funAllLightOn(1);
				delay(40);

				digitalWrite(pin1r,0);
				digitalWrite(pin1g,1);
				digitalWrite(pin1b,0);
				funAllLightOff(1);
				delay(60);
	
				digitalWrite(pin1r,0);
				digitalWrite(pin1g,0);
				digitalWrite(pin1b,1);				
				funAllLightOn(1);
				delay(40);

				digitalWrite(pin1r,1);
				digitalWrite(pin1g,1);
				digitalWrite(pin1b,0);			
				funAllLightOff(1);
				delay(60);

				digitalWrite(pin1r,1);
				digitalWrite(pin1g,0);
				digitalWrite(pin1b,1);
				funAllLightOn(1);
				delay(40);

				digitalWrite(pin1r,0);
				digitalWrite(pin1g,1);
				digitalWrite(pin1b,1);			
				funAllLightOff(1);
				delay(60);
		
				digitalWrite(pin1r,1);
				digitalWrite(pin1g,1);
				digitalWrite(pin1b,1);			
				funAllLightOn(1);
				delay(40);
				funAllLightOff(0);
			}
			funAllLightOn(0);
			delay(400);
			funAllLightOff(0);
		}		
		
		chargeCnt	= 0;		// init value
		chargeShot	= 0;		// init value
	}

}

// LED Btn Thread
void* ledBtnThread(void *arg)
{
	int chargeCnt = 0;	
	int k = 0;
	
	while(1)
	{			
		funRainbowLed();
		usleep(1000);
		
		if( ( millis() - sStopTime ) > stayTime )	// rotation check
		{
			rotationFlag = 1;
		}
		
	}
	
}

int main (int argc, char **argv)
{
	int fd, i;
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	char name[NAME_LENGTH] = "Unknown";
	
	// thread value
	int res;
	pthread_t a_thread;
	void* thread_result;
	
	if (argc != 2) 
	{
		puts("");
		puts("Usage: RainbowLedBtn <#joystick>");
		puts("");
		puts("pin number( GPIO ) : LED(6 12 13 16 19) RGB(20 21 26)");		
		puts("");
		puts("ex) ./RainbowLedBtn /dev/input/js0");
		puts("");
		exit(1);
	}
	
	if ((fd = open(argv[argc - 1], O_RDONLY)) < 0) {
		perror("jstest");
		return 1;
	}

	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);

	//printf("Driver version is %d.%d.%d.\n",		version >> 16, (version >> 8) & 0xff, version & 0xff);

	//printf("Testing ... (interrupt to exit)\n");


/*
 * Event interface, single line readout.
 */

	if (argc == 2) {

		int *axis;
		char *button;
		int i;
		struct js_event js;

		res = pthread_create(&a_thread, NULL, ledBtnThread, (void*)NULL);
			if( res )
		printf("thread create error!\n");

		if( wiringPiSetupGpio() == -1 )
			return 0;
		
		pinMode(pin1r, OUTPUT);	
		pinMode(pin1g, OUTPUT);	
		pinMode(pin1b, OUTPUT);	
		pinMode(pin2, OUTPUT);	
		pinMode(pin3, OUTPUT);	
		pinMode(pin4, OUTPUT);	
		pinMode(pin5, OUTPUT);	
		pinMode(pin6, OUTPUT);	
		
		axis = calloc(axes, sizeof(int));
		button = calloc(buttons, sizeof(char));

		while (1) {
			if (read(fd, &js, sizeof(struct js_event)) != sizeof(struct js_event)) {
				perror("\njstest: error reading");
				return 1;
			}

			switch(js.type & ~JS_EVENT_INIT) {
			case JS_EVENT_BUTTON:
				button[js.number] = js.value;
				break;
			case JS_EVENT_AXIS:
				axis[js.number] = js.value;
				break;
			}

			//printf("\r");

			/*
			if (axes) {
				printf("Axes: ");
				for (i = 0; i < axes; i++)
					printf("%2d:%6d ", i, axis[i]);
			}
			*/

			if (buttons) { 
				for (i = 0; i < buttons; i++)
				{
					//printf("%2d:%s ", i, button[i] ? "on " : "off");
					
					sStopTime = millis();		// rotation time Start
					rotationFlag = 0;		// stop rotation

					if( i==0 && button[i] == 1 )
					{
						if( RGBbtnPress  != 1 )
						{
							RGBbtnPress  = 1;							
							sTime = millis();
						}
					}
					if( i==0 && button[i] == 0 )
					{						
						RGBbtnPress  = 0;							
						sTime = millis();
						digitalWrite(pin1r,0);		// RGB Led Off
						digitalWrite(pin1g,0);		// RGB Led Off
						digitalWrite(pin1b,0);		// RGB Led Off		
					}

					
					// B 버튼
					if( i==1 && button[i] == 1 )
						digitalWrite(pin2,1);					
					if( i==1 && button[i] == 0 && chargeShot == 0 )
						digitalWrite(pin2,0);

					// X 버튼
					if( i==3 && button[i] == 1 )
						digitalWrite(pin3,1);
					if( i==3 && button[i] == 0  && chargeShot == 0 )
						digitalWrite(pin3,0);

					// Y 버튼
					if( i==4 && button[i] == 1 )
						digitalWrite(pin4,1);
					if( i==4 && button[i] == 0 && chargeShot == 0 )
						digitalWrite(pin4,0);

					// L 버튼
					if( i==6 && button[i] == 1 )
						digitalWrite(pin5,1);
					if( i==6 && button[i] == 0 && chargeShot == 0 )
						digitalWrite(pin5,0);

					// R 버튼
					if( i==7 && button[i] == 1 )
						digitalWrite(pin6,1);
					if( i==7 && button[i] == 0 && chargeShot == 0 )
						digitalWrite(pin6,0);

					// Select 버튼( Coin )
					if( i==10 && button[i] == 1 )
					{
						//digitalWrite(pin1g,1);
						funAllLightOn(0);
						delay(40);
						//digitalWrite(pin1g,0);
						funAllLightOff(0);
						delay(40);
						//digitalWrite(pin1g,1);
						funAllLightOn(0);
						delay(40);
						//digitalWrite(pin1g,0);
						funAllLightOff(0);
						delay(40);
						//digitalWrite(pin1g,1);
						funAllLightOn(0);
						delay(40);
						//digitalWrite(pin1g,0);
						funAllLightOff(0);
					}				
				}
			}

			fflush(stdout);
		}
	}

	printf("jstest: unknown mode: %s\n", argv[1]);
	return -1;
}
