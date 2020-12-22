#pragma once
#include <chrono>
#include <vector>
#include <string>

//#include "GraphicFont.h"

class GraphicFont;

class Countdown {
protected:
	float adjustTime;  // by default it counts down in seconds
	std::vector<std::string> whatToShow;
	float fontSize;
	float locX, locY;
	GraphicFont* theFont;
	float scale, angle;

public:
	Countdown() {};
	Countdown(const std::string& commaDelimitedOutputs);

	void setFontSize(float size) { fontSize = size; }
	void setAdjustTime(float adjust) { adjustTime = adjust; }
	void setFont(GraphicFont *aFont) { theFont = aFont; }
	void setLocation(float xLoc, float yLoc) { locX = xLoc; locY = yLoc; }

	float getDuration() { return whatToShow.size() * adjustTime; }
	bool isDone(std::chrono::system_clock::time_point startTime);

	virtual void setScaleAndAngle(float fraction = 1.f);
	virtual void draw(std::chrono::system_clock::time_point startTime);

};

class CountdownSpinner : public Countdown {

public:
	CountdownSpinner(const std::string& commaDelimitedOutputs)
		: Countdown(commaDelimitedOutputs) {}
	virtual void setScaleAndAngle(float fraction = 1.f);

}; 

class CountdownFlyBack : public Countdown {

public:
	CountdownFlyBack(const std::string& commaDelimitedOutputs)
		: Countdown(commaDelimitedOutputs) {}
	virtual void setScaleAndAngle(float fraction = 1.f);

}; 

class CountdownSpinFlyBack : public Countdown {

public:
	CountdownSpinFlyBack(const std::string& commaDelimitedOutputs)
		: Countdown(commaDelimitedOutputs) {}
	virtual void setScaleAndAngle(float fraction = 1.f);

};

class CountdownAnalog : public Countdown {

private:
	int totalTime;
	double startTheta;
	double dTheta;
	double currTheta;
	double rad;
	double width;
public:
	
	CountdownAnalog(int time);
	virtual void draw(std::chrono::system_clock::time_point startTime);

};