#include <gui/screen1_screen/Screen1View.hpp>
#include "cmsis_os2.h"

Screen1View::Screen1View()
{
	tickCount = 0;
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

extern osMessageQueueId_t Queue1Handle;

void Screen1View::handleTickEvent()
{
    static uint32_t prevTick = 0;
    Screen1ViewBase::handleTickEvent();
    // Xử lí theo giây
    tickCount = osKernelGetTickCount();
    uint32_t seconds = tickCount/osKernelGetTickFreq();
    float rad = 0;
	uint32_t count = osMessageQueueGetCount(Queue1Handle);
	uint8_t res = 0;
	if (count > 0)
	{
		osMessageQueueGet(Queue1Handle, &res, NULL, osWaitForever);
		if (res == 'P')
		{
			rad = (seconds % 60) * 6.0f * 3.14159f / 180.0f;
			textureMapper1.updateZAngle(rad);
			textureMapper1.invalidate();
		}
		else
		{
			prevTick = tickCount;
		}
	}

}

