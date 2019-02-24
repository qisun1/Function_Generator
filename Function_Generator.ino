//Library: https://github.com/Billwilliams1952/AD9833-Library-Arduino
//Connection: Arduino D11 (MOSI) - > AD9833 SDATA
//Connection: Arduino D13 (SCK) - > AD9833 SCLK
//Connection: Arduino D5 (FSYNC) - > AD9833 FSYNC


#include <digitalWriteFast.h>
#include <AD9833.h>
#include <SPI.h>

#include <Wire.h>  // required for LiquidCrystal_I2C
#include <Encoder.h>  // * Paul Stoffregen https://github.com/PaulStoffregen/Encoder
#include <LiquidCrystal_I2C.h>  // * John Rickman https://github.com/johnrickman/LiquidCrystal_I2C
#include <FastX9CXXX.h> // * GitMoDu https://github.com/GitMoDu/FastX9CXXX
#include <Fast.h> //* GitMoDu https://github.com/GitMoDu/Fast


#define FNC_PIN  7
#define LCD_brightness 5

// pins for rotary encoder
#define rotaryPin1 3 
#define rotaryPin2 2
#define buttonPin 4

//pins for digital pot x9c103 (10k) for LM317T voltage regulator
#define X9_CS_PIN A2
#define X9_UD_PIN A1
#define X9_INC_PIN A0

uint32_t frq;
uint8_t amplitude;
bool changedFlagWave;
bool changedFlagAmplitue;

int8_t menuCursorPos; //menu :home; 2:WAVE; 3:Frequency; 4:meters
bool menuChangeValue;
String waveFormString;
WaveformType waveType;

unsigned long lastButtonPress;
const uint8_t step = 4;
//buffer for format string
char buffer[8];

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 20 chars and 4 line display
AD9833 gen(FNC_PIN);
Encoder myEnc(rotaryPin1, rotaryPin2);
FastX9C102 PotAmplitue;

//pot setting to amplitude mv
//formula: amplitude = PotSetting *6.9-15.9
const float coeff = 6.9;
const float intercept = -15.9;

void setup()
{
	Serial.begin(9600);
	delay(10);
	Serial.println("initialize");
	//Start LCD Display	
	lcd.setBacklight((uint8_t)1);
	lcd.init();
	pinMode(LCD_brightness, OUTPUT);
	analogWrite(LCD_brightness, 100);

	//Initialize values and menu
	init_system();

	//// initialize pot
	PotAmplitue.Setup(X9_CS_PIN, X9_UD_PIN, X9_INC_PIN);
	PotAmplitue.JumpToStep(amplitude);


	////set encoder
	pinMode(buttonPin, INPUT_PULLUP);
	pinMode(rotaryPin1, INPUT_PULLUP);
	pinMode(rotaryPin2, INPUT_PULLUP);
	lastButtonPress = millis();

	//Start AD9833
	// This MUST be the first command after declaring the AD9833 object
	waveType = SINE_WAVE;
	frq = 1000;
	gen.Begin();              // The loaded defaults are 1000 Hz SINE_WAVE using REG0
	gen.ApplySignal(waveType, REG1, frq);
	gen.EnableOutput(true);  // Turn ON the output
	delay(10);
	Serial.println("ok now");
	Serial.println("ok start");
}

// Add the main program code into the continuous loop() function
void loop()
{
	if ((millis() - lastButtonPress) > 500)
	{
		long newPosition = myEnc.read();
		if (newPosition != 0)
		{
			Serial.println(newPosition);
		}
		
		if (newPosition >= step) {
			myEnc.write(0);
			moveCursor(1);

		}
		else if (newPosition <= -step)
		{
			myEnc.write(0);
			moveCursor(-1);
		}


		if (digitalRead(buttonPin) == LOW)
		{
			lastButtonPress = millis();
			buttonPressed();
		}
	}
	//Serial.println("loop");
	delay(1);
}

void init_system()
{
	menuChangeValue = false;
	menuCursorPos = 0;
	waveFormString = "SINE       ";
	waveType = SINE_WAVE;
	frq = 1000;
	amplitude = 50;

	lcd.setCursor(1, 0);
	lcd.print(waveFormString);

	lcd.setCursor(1, 1);

	printfrq();
	printamplitude();

	lcd.setCursor(0, 0);
	lcd.print("*");
}

void buttonPressed()
{
	if (menuChangeValue == false)
	{
		changedFlagWave = false;
		changedFlagAmplitue = false;
		menuChangeValue = true;
		setCursorPos();
		lcd.print("-");		
	}
	else
	{
		if (changedFlagWave == true)
		{
			gen.ApplySignal(waveType, REG1, frq);
		}

		if (changedFlagAmplitue == true)
		{

		}

		menuChangeValue = false;
		setCursorPos();
		lcd.print("*");
	}
}

void moveCursor(int8_t moveNext)
{

	if (menuChangeValue == false)
	{
		setCursorPos();
		lcd.print(" ");

		menuCursorPos += moveNext;
		if (menuCursorPos > 2)
		{
			menuCursorPos = 0;
		}
		else if (menuCursorPos < 0)
		{
			menuCursorPos = 2;
		}

		setCursorPos();
		lcd.print("*");

	}
	else
	{
		switch (menuCursorPos) 
		{
			case 0:	
				//change wave form
				changedFlagWave = true;
				if (waveType == SINE_WAVE)
				{
					waveFormString =  "SQUARE     ";
					waveType = SQUARE_WAVE;
				}
				else if (waveType == SQUARE_WAVE)
				{
					waveFormString = "HALF SQUARE" ;
					waveType = HALF_SQUARE_WAVE;
				}
				else if (waveType == HALF_SQUARE_WAVE)
				{
					waveFormString = "TRIANGLE   ";
					waveType = TRIANGLE_WAVE;
				}
				else if (waveType == TRIANGLE_WAVE)
				{
					waveFormString = "SINE       ";
					waveType = SINE_WAVE;
				}
				lcd.setCursor(1, 0);
				lcd.print(waveFormString);
				break;
			case 1:
				//change frequency value
				changedFlagWave = true;
				if (moveNext > 0)
				{
					if (frq <= 900)
					{
						frq += 100;
					}
					else if ((frq >= 1000) && (frq <= 9000))
					{
						frq += 1000;
					}
					else if ((frq >= 10000) && (frq <= 90000))
					{
						frq += 10000;
					}
					else if ((frq >= 100000) && (frq <= 900000))
					{
						frq += 100000;
					}
				}
				else
				{
					if ((frq <= 1000) && (frq >100))
					{
						frq -= 100;
					}
					else if ((frq > 1000) && (frq <= 10000))
					{
						frq -= 1000;
					}
					else if ((frq > 10000) && (frq <= 100000))
					{
						frq -= 10000;
					}
					else if ((frq > 100000) && (frq <= 1000000))
					{
						frq -= 100000;
					}
				}
				printfrq();
				break;
			case 2:
				//change amplitude value
				changedFlagAmplitue = true;
				if ((moveNext > 0) && (amplitude<99))
				{
					amplitude += 1;
					PotAmplitue.Up();
				}
				else if ((moveNext < 0) && (amplitude > 4))
				{
					amplitude -= 1;
					PotAmplitue.Down();
				}
				printamplitude();
				break;
		}
	}
}

void setCursorPos()
{
	switch (menuCursorPos)
	{
		case 0:
			lcd.setCursor(0, 0);
			break;
		case 1:
			lcd.setCursor(0, 1);
			break;
		case 2:
			lcd.setCursor(8, 1);
			break;
	}
}

void printfrq()
{
	if (frq >= 1000)
	{

		snprintf(buffer, 8, "%3d KHz", int(frq / 1000));
	}
	else
	{
		snprintf(buffer, 8, "%3d Hz ", frq);
	}
	lcd.setCursor(1, 1);
	lcd.print(buffer);
}

void printamplitude()
{
	int display = int(coeff * amplitude + intercept);
	snprintf(buffer, 8, "%4d mV", display);
	lcd.setCursor(9, 1);
	lcd.print(buffer);
}