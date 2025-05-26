#include <gui/screen2_screen/Screen2View.hpp>
#include "cmsis_os.h"
#include "main.h"


extern osMessageQueueId_t Queue1Handle;

uint32_t xorshift32()
{
    static uint32_t seed = 0;

    if (seed == 0)
        seed = HAL_GetTick();  // hoặc giá trị ADC noise nếu có

    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;

    return seed;
}


Screen2View::Screen2View()
    : isGameOver(false), fallSpeed(2), speedLevel(0),carSpeedStep(2)
{
    tickCount = 0;
}


bool Screen2View::checkCollision()
{
    int16_t x1 = image1.getX();
    int16_t y1 = image1.getY();
    int16_t w1 = image1.getWidth();
    int16_t h1 = image1.getHeight();

    int16_t x2 = lamb.getX();
    int16_t y2 = lamb.getY();
    int16_t w2 = lamb.getWidth();
    int16_t h2 = lamb.getHeight();

    return !(x1 + w1 < x2 || x2 + w2 < x1 ||
             y1 + h1 < y2 || y2 + h2 < y1);
}

void Screen2View::setupScreen()
{
    localImageX = presenter->GetImageX();
    Screen2ViewBase::setupScreen();
    image1.setX(localImageX);
    lamb.setX(14);
    lamb.setY(0);
    score = 0;
    highScore = 0;

}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
    presenter->UpdateImageX(localImageX);
}

void Screen2View::handleTickEvent()
{
	Screen2ViewBase::handleTickEvent();

	if (isGameOver)
	{
	    uint8_t msg;
	    if (osMessageQueueGet(Queue1Handle, &msg, NULL, 0) == osOK && msg == 'A')
	    {
	        // Reset lại trạng thái game
	    	score = 0;

	    	Unicode::snprintf(scoreBuffer, sizeof(scoreBuffer), "%d", score);
	    	textScore.setWildcard(scoreBuffer);
	    	textScore.invalidate();

	    	isGameOver = false;
	    	tickCount = 0;
	    	fallSpeed = 2;
	    	speedLevel = 0;
	    	localImageX = 100;
	    	carSpeedStep = 2;
	    	lamb.setY(0);

	        lamb.setVisible(true);
	        image1.setX(localImageX);
	        textGameOver.setVisible(false);

	        // Hiện lại đường
	        track0.setVisible(true);
	        track1.setVisible(false);
	        track2.setVisible(false);
	        track3.setVisible(false);
	        track4.setVisible(false);

	        image1.invalidate();
	        lamb.invalidate();
	        textGameOver.invalidate();
	    }

	    return;
	}

	uint8_t msg;
	if (osMessageQueueGet(Queue1Handle, &msg, NULL, 0) == osOK)
	{
	    const int16_t minX = 0;
	    const int16_t maxX = 240 - image1.getWidth();

	    if (msg == 'L' && localImageX > minX)
	    {
	        localImageX -= carSpeedStep;
	    }
	    else if (msg == 'R' && localImageX < maxX)
	    {
	        localImageX += carSpeedStep;
	    }

	    image1.setX(localImageX);
	    image1.invalidate();
	}

	tickCount++;

	switch (tickCount % 5)
	{
	case 0:
		track0.setVisible(true);
		track4.setVisible(false);
		break;
	case 1:
		track1.setVisible(true);
		track0.setVisible(false);
		break;
	case 2:
		track2.setVisible(true);
		track1.setVisible(false);
		break;
	case 3:
		track3.setVisible(true);
		track2.setVisible(false);
		break;
	case 4:
		track4.setVisible(true);
		track3.setVisible(false);
		break;
	default:
		break;
	}

	int newY = lamb.getY() + fallSpeed;
	if (newY >= 320)
	{
	    newY = 0;
	    score++;
	    if (score > highScore)
	        {
	            highScore = score;
	        }
	    // Cập nhật điểm
	    Unicode::snprintf(scoreBuffer, sizeof(scoreBuffer), "%d", score);
	    textScore.setWildcard(scoreBuffer);
	    textScore.invalidate();

	    Unicode::snprintf(highScoreBuffer, sizeof(highScoreBuffer), "%d", highScore);
	    textHighScore.setWildcard(highScoreBuffer);
	    textHighScore.invalidate();
	    int index = xorshift32() % 4;
	    int newX = index * 60 + 15;
	    lamb.setX(newX);
	    lamb.invalidate();
	}
	lamb.setY(newY);

	if (checkCollision())
	{
	    isGameOver = true;

	    textGameOver.setVisible(true);
	    lamb.setVisible(false);
	    textGameOver.invalidate();
	}


	// Cứ mỗi 400 tick tăng tốc một lần
	if (tickCount % 400 == 0 && fallSpeed < 5)
	{
	    fallSpeed++;
	    speedLevel++;
	    carSpeedStep = fallSpeed;
	}

	invalidate();
}
