#include "main.h"


//#######################################################
#define EX1 1
#define EX2 2
#define EX3 3
#define US 4

#define SYNCHRO_EX US

#define ID_ROUES 'R'
#define ID_ROUES_UP 'A'
#define ID_ROUES_DOWN 'R'
#define ID_ROUES_LEFT 'G'
#define ID_ROUES_RIGHT 'D'
#define ID_ROUES_STOP 'S'

#define ID_TOURELLE 'T'
#define ID_TOURELLE_UP 'A'
#define ID_TOURELLE_DOWN 'R'
#define ID_TOURELLE_LEFT 'G'
#define ID_TOURELLE_RIGHT 'D'
#define ID_TOURELLE_STOP 'S'

#define ID_CONSIGNE 'C'

//#######################################################

// Déclaration des objets synchronisants !! Ne pas oublier de les créer
xSemaphoreHandle xSemaphore = NULL;
xSemaphoreHandle semA = NULL;
xQueueHandle qh = NULL;


extern uint8_t rec_buf2[NB_CAR_TO_RECEIVE+1];	 // defined in drv_uart.c
extern uint8_t rec_buf6[NB_CAR_TO_RECEIVE+1];

struct AMessage
{
	char command;
	int data;
};

struct SensorMsg
{
	int AV_Left;
	int AV_Right;
	int AR_Left;
	int AR_Right;
};



int dist_IR[2];
int consigne;
int cam_status = 1; // 0: ok, 1: research, 2: error
int cam_cmd_x = 70;
int cam_cmd_y = 70;

int motor_left_cmd = 0;
int motor_right_cmd = 0;

float Kp_L = 0.005;
float Ki_L = 0.75; //0.003/(0.1*0.02) // Te/(

float Kp_R = 0.005;
float Ki_R = 0.75; //0.003/(0.1*0.02) // Te/(
int test_cmd[1000];
int test_index=0;

int led = 0;

//========================================================
#if SYNCHRO_EX == EX1

static void task_A(void *pvParameters)
{


	for (;;)
	{
		//term_printf("TASK A \n\r");

		// DISTANCE

		captDistIR_Get(dist_IR);

		//term_printf("dist_G: %d \t|\t dist_D: %d \n\r", dist_IR[0], dist_IR[1]);
		/*
		consigne = (int)(dist_IR[1]*200/3200);
		if (consigne>200) {
			consigne = 200;
		}
		motorRight_SetDuty(consigne);

		int commandeServo;
		commandeServo= dist_IR[1] *120/3200;

		if(commandeServo > 120)
		{
			commandeServo = 120;
		}

		if(commandeServo < 20)
		{
			commandeServo = 20;
		}

		servoHigh_Set(commandeServo);
		servoLow_Set(commandeServo);

		*/
		asservLeftMotor(100);
		asservRightMotor(100);
		//tracking();
		vTaskDelay(3); // 1000 ms

	}

}

static void task_B(void *pvParameters)
{
	for (;;)
	{
		//term_printf("TASK B \n\r");
	}
}
//========================================================
#elif SYNCHRO_EX == EX2

static void task_C( void *pvParameters )
{
	for (;;)
	{
		term_printf("TASK C \n\r");
		xSemaphoreTake( xSemaphore, portMAX_DELAY );
	}
}

static void task_D( void *pvParameters )
{
	for (;;)
	{
		term_printf("TASK D \n\r");
		xSemaphoreGive( xSemaphore );
	}
}

//========================================================
#elif SYNCHRO_EX == EX3

static void task_E( void *pvParameters )
{
	struct AMessage pxMessage;
	pxMessage.command='a';
	pxMessage.data=10;
	vTaskDelay(1000);
	for (;;)
	{
	    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, 1);
		term_printf("TASK E \n\r");
		//xQueueSend( qh, ( void * ) &pxMessage,  portMAX_DELAY );
		xSemaphoreTake( xSemaphore, portMAX_DELAY );
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, 0);
		vTaskDelay(SAMPLING_PERIOD_ms);

	}
}

static void task_F(void *pvParameters)
{
	struct AMessage pxRxedMessage;

	for (;;)
	{
		xQueueReceive( qh,  &( pxRxedMessage ) , portMAX_DELAY );
		term_printf("TASK F \n\r");
		xSemaphoreGive( xSemaphore );
	}
}
#elif SYNCHRO_EX == US

	static void task_motor( void *pvParameters )
	{
		for(;;)
		{
			term_printf("TASK MOTOR \n\r");
		}
	}

	static void task_sensors( void *pvParameters )
	{
		int loop = 0; //70   -> avec vtaskdelay => 24 loop
		struct SensorMsg pxMessage;
		pxMessage.AV_Left=0;
		pxMessage.AV_Right=0;
		pxMessage.AR_Left=0;
		pxMessage.AR_Right=0;
		vTaskDelay(70);
		for(;;)
		{



			//term_printf("TASK SENSORS \n\r");
			captDistIR_Get(dist_IR);

			captDistUS_Measure(0xE0);
			if (loop > 23) {
			uint16_t dist = captDistUS_Get(0xE0);
			term_printf("dist: %d \n\r", dist);
			loop = 0;
			}
			loop++;

			pxMessage.AV_Left=(dist_IR[0] > 2000);
			pxMessage.AV_Right=(dist_IR[1] > 2000);
			pxMessage.AR_Left=0;
			pxMessage.AR_Right=0;

			xQueueSend( qh, ( void * ) &pxMessage,  portMAX_DELAY );
			xSemaphoreTake( semA, portMAX_DELAY );

			vTaskDelay(70);
		}
	}

	static void task_cam( void *pvParameters )
	{
		for(;;)
		{
			term_printf("TASK CAM \n\r");
		}
	}

	static void task_main( void *pvParameters )
	{
		struct AMessage pxRxedMessage;

		for (;;)
		{
			xQueueReceive( qh,  &( pxRxedMessage ) , portMAX_DELAY );
			asservLeftMotor(500);
			asservRightMotor(500);

			xSemaphoreGive( semA );
		}

		/*for(;;)
		{
			//term_printf("TASK MAIN \n\r");
			xSemaphoreTake(semA, portMAX_DELAY);

			asservLeftMotor(500);
			asservRightMotor(500);
			//tracking();
			vTaskDelay(3); // 1000 ms

		}*/
	}

#endif
//=========================================================
//	>>>>>>>>>>>>	MAIN	<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//=========================================================

int main(void)
{
	HAL_Init();
	SystemClock_Config();

	uart2_Init();			// CABLE
	uart6_Init();			// ZIGBEE
	i2c1_Init();
	spi1Init();
	pixyCam_Init();			// Caméra Pixy
	captDistIR_Init();		// Capteurs Infrarouge
	quadEncoder_Init();		// Encodeurs Incrémentaux
	motorCommand_Init();	// Commande des Hacheurs
	servoCommand_Init();	// Commande des Servomoteurs

	HAL_Delay(1000);

	motorLeft_SetDuty(100);
	motorRight_SetDuty(100);

#if SYNCHRO_EX == EX1
	xTaskCreate( task_A, ( const portCHAR * ) "task A", 512 /* stack size */, NULL, tskIDLE_PRIORITY+2, NULL );
	xTaskCreate( task_B, ( const portCHAR * ) "task B", 512 /* stack size */, NULL, tskIDLE_PRIORITY+1, NULL );
#elif SYNCHRO_EX == EX2
	xTaskCreate( task_C, ( signed portCHAR * ) "task C", 512 /* stack size */, NULL, tskIDLE_PRIORITY+2, NULL );
	xTaskCreate( task_D, ( signed portCHAR * ) "task D", 512 /* stack size */, NULL, tskIDLE_PRIORITY+1, NULL );
#elif SYNCHRO_EX == EX3
	xTaskCreate( task_E, ( signed portCHAR * ) "task E", 512 /* stack size */, NULL, tskIDLE_PRIORITY+2, NULL );
	xTaskCreate( task_F, ( signed portCHAR * ) "task F", 512 /* stack size */, NULL, tskIDLE_PRIORITY+1, NULL );
#elif SYNCHRO_EX == US
	xTaskCreate( task_main, 	( signed portCHAR * ) "task Main"	, 512, NULL, tskIDLE_PRIORITY+4, NULL );
	xTaskCreate( task_sensors, 	( signed portCHAR * ) "task Sensors", 512, NULL, tskIDLE_PRIORITY+3, NULL );
	//xTaskCreate( task_motor,	( signed portCHAR * ) "task Motor"	, 512, NULL, tskIDLE_PRIORITY+2, NULL );
	//xTaskCreate( task_cam, 		( signed portCHAR * ) "task Cam"	, 512, NULL, tskIDLE_PRIORITY+1, NULL );

#endif

	//vSemaphoreCreateBinary(xSemaphore);
	vSemaphoreCreateBinary(semA);

	xSemaphoreTake( semA, portMAX_DELAY );

	qh = xQueueCreate( 1, sizeof(struct AMessage ) );

	vTaskStartScheduler();

	return 0;

}


//=================================================================
//	UART RECEIVE CALLBACK5
//=================================================================
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle -> Instance == USART2)
	{

		 HAL_UART_Receive_IT(&Uart2Handle, (uint8_t *)rec_buf2, 3); // 3 est le nombre de caracteres pour déclencher l'interupt
		 if(*rec_buf2 == 0x41) // == A
		 {
			 servoHigh_Set(20);
		 }
		 else if(*rec_buf2 == 0x52) // == R
		 {
			 servoHigh_Set(120);
		 }
		 else if(*rec_buf2 == 0x47) // == G
		 {
			 servoLow_Set(120);
		 }
		 else if(*rec_buf2 == 0x44) // == D
		 {
			 servoLow_Set(20);
		 }

		 else if(*rec_buf2 == 0x53) // == S
		 {
			 servoHigh_Set(70);
			 servoLow_Set(70);
		 }
		 else
		 {
			 // Nothing to do
		 }

	}

	if(UartHandle -> Instance == USART6)
	{

		 HAL_UART_Receive_IT(&Uart6Handle, (uint8_t *)rec_buf6, NB_CAR_TO_RECEIVE);
		 if(*rec_buf2 == 0x5A) // == Z
		 {
			 servoHigh_Set(20);
		 }
		 servoLow_Set(120);
	}

}



//=================================================================
// Called if stack overflow during execution
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed char *pcTaskName)
{
	//term_printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	/* If the parameters have been corrupted then inspect pxCurrentTCB to
	 * identify which task has overflowed its stack.
	 */
	for (;;) {
	}
}
//=================================================================
//This function is called by FreeRTOS idle task
extern void vApplicationIdleHook(void)
{
}
//=================================================================
// brief This function is called by FreeRTOS each tick
extern void vApplicationTickHook(void)
{
//	HAL_IncTick();
}


void asservLeftMotor(int consigne)
{
	int speed = quadEncoder_GetSpeedL();

	static float erreur = 0;
	static float up = 0;
	static float ui = 0;

	erreur = consigne - speed;
	up = Kp_L * erreur;
	ui = Ki_L * up + ui;
	motor_left_cmd = 100 + ui + up;

	if (motor_left_cmd > 200)  { motor_left_cmd = 200; }
	if (motor_left_cmd < 0) 	{ motor_left_cmd = 0;   }


	if( test_index < 1000)
	{
	test_cmd[test_index] = speed;
	test_index++;
	}

	motorLeft_SetDuty(motor_left_cmd);
}

void asservRightMotor(int consigne)
{
	int speed = quadEncoder_GetSpeedR();

	static float erreur = 0;
	static float up = 0;
	static float ui = 0;

	erreur = consigne - speed;
	up = Kp_R * erreur;
	ui = Ki_R * up + ui;
	motor_right_cmd = 100 + ui + up;

	if (motor_right_cmd > 200)  { motor_right_cmd = 200; }
	if (motor_right_cmd < 0) 	{ motor_right_cmd = 0;   }

	if( test_index < 1000)
	{
	test_cmd[test_index] = speed;
	test_index++;
	}

	motorRight_SetDuty(motor_right_cmd);
}

void tracking()
{
	static int32_t nbBlock = -1;
	nbBlock = pixyCam_GetBlocks(1);

	if (nbBlock)
	{

		uint16_t XCamWidth, YCamHeight, XCamCenter, YCamCenter;
		int XDiff, YDiff;
		XCamWidth = 260;
		YCamHeight = 280;

		uint16_t blockPosition[2], blockSize[2];  // position x, y and width, height
		pixyCam_Get(blockPosition, blockSize);

		uint16_t xObject, yObject, wObject, hObject;
		xObject = blockPosition[0];  // center of object
		yObject = blockPosition[1];
		wObject = blockSize[0];
		hObject = blockSize[1];

		// calcul center of cam window
		XCamCenter = (uint16_t) (XCamWidth)/2;
		YCamCenter = (uint16_t) (YCamHeight)/2;

		// calcul diff between object and cam center
		XDiff = xObject - XCamCenter;
		YDiff = yObject - YCamCenter;

		term_printf("XDiff: %d, YDiff: %d\n", XDiff, YDiff);


		if (XDiff>150 && cam_cmd_x>20) {
			cam_cmd_x --;
		}
		if (XDiff<150 && cam_cmd_x<120) {
			cam_cmd_x ++;
		}

		if (YDiff>150 && cam_cmd_y>20) {
			cam_cmd_y ++;
		}
		if (YDiff<150 && cam_cmd_y<120) {
			cam_cmd_y --;
		}

		/*if (XDiff>0 && YDiff>0) {
			term_printf("BAS DROITE\n");
		}
		if (XDiff<0 && YDiff>0) {
			term_printf("BAS GAUCHE\n");
		}
		if (XDiff<0 && YDiff<0) {
			term_printf("HAUT GAUCHE\n");
		}
		if (XDiff>0 && YDiff<0) {
			term_printf("HAUT DROITE\n");
		}*/

		servoLow_Set(cam_cmd_x);
		servoHigh_Set(cam_cmd_y);
	}
}
