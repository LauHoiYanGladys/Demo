#include <vector>
#include <iostream>
#include "Countdown.h"
#include "StringPlus.h"
#include "GraphicFont.h"
#include "DrawingUtilNG.h"

using namespace std;
const double PI = 3.14;

Countdown::Countdown(const string& commaDelimitedOutputs)
{
	whatToShow = StringPlus::split(commaDelimitedOutputs, ",");
	adjustTime = 1.0;
	theFont = nullptr;
}

bool Countdown::isDone(std::chrono::system_clock::time_point startTime)
{
	double elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::system_clock::now() - startTime).count() / 1000. / adjustTime;
	int index = elapsedTime;

	return int(index) >= whatToShow.size();
}

void Countdown::setScaleAndAngle(float fraction)
{
	scale = fontSize / theFont->getLetterHeight();
	angle = 0;
}

void Countdown::draw(std::chrono::system_clock::time_point startTime)
{
	if (theFont != nullptr) {
		double elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::system_clock::now() - startTime).count() / 1000. / adjustTime;
		int index = elapsedTime;

		if (index < whatToShow.size()) {
			double fadeFraction = 1 - (elapsedTime - index) / 1.;
			theFont->setFade(fadeFraction);
			setScaleAndAngle(fadeFraction); // calls virtual function
			theFont->drawText(whatToShow.at(index), locX, locY, scale, angle, true);
		}
	}
}


void CountdownSpinner::setScaleAndAngle(float fraction)
{
	scale = fontSize / theFont->getLetterHeight();
	angle = 15.f - 120.f * (1. - fraction);
}

void CountdownFlyBack::setScaleAndAngle(float fraction)
{
	scale = fontSize / theFont->getLetterHeight() * fraction;
	angle = 0.f;
}

void CountdownSpinFlyBack::setScaleAndAngle(float fraction)
{
	scale = fontSize / theFont->getLetterHeight() * fraction;
	angle = 15.f - 120.f * (1. - fraction);
}

void CountdownAnalog::draw(std::chrono::system_clock::time_point startTime)
{
	double elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::system_clock::now() - startTime).count() / 1000. / adjustTime;
	int index = elapsedTime;

	currTheta = startTheta - dTheta * index;
	rad = fontSize * 1.;
	width = fontSize * 0.15;

	double leftUpperX = locX - 0.5 * width * (sin(currTheta));
	double leftUpperY = locY - 0.5 * width * (cos(currTheta));
	double leftLowerX = locX + 0.5 * width * (sin(currTheta));
	double leftLowerY = locY + 0.5 * width * (cos(currTheta));
	double rightLowerX = leftLowerX + rad * cos(currTheta);
	double rightLowerY = leftLowerY - rad * sin(currTheta);
	double rightUpperX = leftUpperX + rad * cos(currTheta);
	double rightUpperY = leftUpperY - rad * sin(currTheta);

	glBegin(GL_QUADS);
	glVertex2d(leftUpperX, leftUpperY);
	glVertex2d(leftLowerX, leftLowerY);
	glVertex2d(rightLowerX, rightLowerY);
	glVertex2d(rightUpperX, rightUpperY);
	glEnd();

}

CountdownAnalog::CountdownAnalog(int time)
{
	startTheta = PI / 2;
	totalTime = time;
	adjustTime = 1.0; // no slowing down or fast forward by default

	dTheta = 2 * PI / time;
	currTheta = PI / 2;
	rad = 100.0;
	width = 10;
}
