// Program Name : 	LEGO NXScannerT
// Author       : 	Tamim Faruk, Tianyu Guo, Sally Hui, Fernando Yee
// Course(s)    : 	MTE100, GENE121
// Date Created : 	November 24, 2015
// Description  : 	This program controls the LEGO NXScannerT. It is a 
//	 			robot which autonomously scans a book using an //				attached phone running CamScanner. 


void pushForward(); 
void pushBackward();
void turnPage();

void takePhoto(int right, int left);

bool isPageJam();
void errorSound();
void fixJam();

bool isFinish();

/** Detects if a page is jammed (cannot lay flat because 
	it's been caught on page pushing motor or flipping arm) 
by TAMIM**/
bool isPageJam()
{
	return (SensorValue[S1]<28);
}

// Plays error sound to alert user about a page jam. By Tamim and Fernando
void errorSound()
{
	playTone(440, 30);
	playTone(466.2, 30);
	playTone(493.9, 30);
	playTone(523.3, 70);

	wait10Msec(160);
}

// Prompts user to fix page jam. By Tianyu
void fixJam()
{
	// Verify motors are stopped
	motor[motorA]=0;
	motor[motorB]=0;
	motor[motorC]=0;

	errorSound(); // alert user
	displayString(0,"Please press any button after");
	displayString (1, "adjustment.");
	
	while(nNxtButtonPressed== -1){} // wait for user to fix
}

// Uses page flipping arm to "turn" each page. By Fernando.
void turnPage()
{
	nMotorEncoder[motorB] = 0;
	motor[motorB] = -40; 
	while (nMotorEncoder[motorB] > -215){} // turns -215 deg
	motor[motorB]=0;
	nMotorEncoder[motorB]=0;
}

/** Rotates phone left and right to take a picture of 
each page. Please refer to flowchart for more detailed visualization. By Tianyu, Sally, and Tamim.**/
void takePhoto(int right, int left)
{
	int initPosition = nMotorEncoder[motorB];
	string read = "0";
	motor[motorB]=-30;
	while (read == "0")
	{
		read = SensorValue[S2];
		wait1Msec(100);
	}

	motor[motorB] = 30;
	while (nMotorEncoder[motorB] < initPosition){}
	motor[motorB]=0;

	wait10Msec(200); // wait for phone to respond

	time1[T1] = 0;
	motor[motorC] = 20;
	while (nMotorEncoder[motorC]<-left + right && time1[T1] < 500){}
	motor[motorC]=0;

	wait10Msec(200); // wait for phone to respond

	nMotorEncoder[motorB] = 0;
	motor[motorB]=-30;

	read = "0";
	while (read == "0")
	{
		read = SensorValue[S2];
		wait1Msec(100);
	}
	int pressPosition = nMotorEncoder[motorB];
	nMotorEncoder[motorB] = 0;
	while (nMotorEncoder[motorB] > 0.1*pressPosition){};

	motor[motorB] = 0;
	motor[motorC] = 0;

	time1[T1] = 0;
	motor[motorC] = -20;
	nMotorEncoder[motorC] = 0;
	while (nMotorEncoder[motorC] > left - right && time1[T1] < 500){}
	motor[motorC] = 0;	
}

/** Determines if the book has been finished; coloured square has been
 	placed right after the last page to be flipped. By Tamim.**/
bool isFinish ()
{
	return SensorValue[S4] == 5;
}


task main()
{
	// Setup sensors
	SensorType[S1] = sensorSONAR;
	SensorType[S4] = sensorColorNxtFULL;
	SensorType[S2] = sensorTouch;
	SensorType[S3] = sensorColorNxtFULL;

	// Phone rotation angles
	int left = -15;
	int right = 15;
	bool bookFinished = false;

	while (!bookFinished)
	{
		// Reset motor encoders
		nMotorEncoder[motorA]=0;
		nMotorEncoder[motorB]=0;
		nMotorEncoder[motorC]=0;

		takePhoto(right, left);

		// Flip page
		pushForward(); 
		turnPage();

		// Return motors to starting position and flatten pages
		pushBackward(); 
		if (isPageJam())
			fixJam();
		wait10Msec(100);

		bookFinished=isFinish();
	}

	// Stop motors
	motor[motorB]=0;
	motor[motorA]=0;
	motor[motorC]=0;
	wait10Msec(100);
}
