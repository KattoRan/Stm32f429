#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/widgets/canvas/Circle.hpp>
#include "main.h"
#include "cmsis_os2.h"
#include <cmath>
Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::buttonClicked()
{
	HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
}

//3.12
//void Screen1View::tickEvent()
//{
//	tickCount += 2;
//	tickCount = tickCount % 240;
////draw circle at the new position
//	circle1.moveTo (tickCount, 50);
//	circle1.invalidate();
//}

//3.13
extern osMessageQueueId_t myQueue01Handle;

void Screen1View:: tickEvent()

{
	tickCount += 1;
	tickCount = tickCount % 240;
	float x = tickCount / 55.0f;
	float y = sin(x)+sin(2*x)+sin(3*x)+1;
	uint8_t res;
	if (osMessageQueueGetCount(myQueue01Handle) > 0)
	{
		osMessageQueueGet(myQueue01Handle, &res, NULL, osWaitForever);

		if (res == 'X'){
				circle1.moveTo((int16_t)floor(x*55),200-(int16_t)floor(y*50));
				circle1.invalidate();
		}

	}

}


