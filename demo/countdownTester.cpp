/*
Nestor Gomez 10/21/20
*/
#include <vector>
#include "fssimplewindow.h"
#include "GraphicFont.h"
#include "Countdown.h"

void drawCrosshairs(int posX, int posY, int size)
{  // draws cross hairs
	glBegin(GL_LINES);
	glVertex2i(posX, posY - size / 2); glVertex2i(posX, posY + size / 2);
	glVertex2i(posX - size / 2, posY); glVertex2i(posX + size / 2, posY);
	glEnd();
}

int main(void)
{
	using namespace std;
	using namespace DrawingUtilNG;

	int key = FSKEY_NULL;
	double positionX[] = { 100, 300, 500, 700 };
	double positionY = 300;

	// must "open window" before declaring fonts
	FsOpenWindow(16, 16, 800, 600, 1);

	ImpactFont impact;
	GaramondFont garamond;
	JokermanFont jokerman;
	ComicSansFont comicsans;
	TimesNewRomanFont timesnewroman;
	OldEnglishFont oldenglish;
	CourierNewFont couriernew;
	ArialFont arial;

	comicsans.setColorHSV(260, 1, .8);

	// in vector, must use pointers to objects to assure virtual functions work
	//     storing directly in vector will produce "object slicing"
	vector<Countdown*> theCountdowns;

	theCountdowns.push_back(new CountdownSpinner("5,4,Are you ready?,On Your Marks,Get Set,Go,Run!"));
	theCountdowns.back()->setFont(&jokerman);
	theCountdowns.back()->setLocation(positionX[0], positionY);
	theCountdowns.back()->setFontSize(40);
	theCountdowns.back()->setAdjustTime(2);  // doubles the time between events (2 seconds instead of 1)

	theCountdowns.push_back(new Countdown("10,9,8,7,6,5,4,3,2,1,Blast Off"));
	theCountdowns.back()->setFont(&couriernew);
	theCountdowns.back()->setLocation(positionX[1], positionY);
	theCountdowns.back()->setFontSize(80);
	//theCountdowns.back()->setAdjustTime(1);

	theCountdowns.push_back(new CountdownSpinFlyBack("10,9,8,7,6,5,4,3,2,1,0,+1,+2,+3,+4,+5"));
	theCountdowns.back()->setFont(&timesnewroman);
	theCountdowns.back()->setLocation(positionX[2], positionY);
	theCountdowns.back()->setFontSize(80);
	//theCountdowns.back()->setAdjustTime(1);

	theCountdowns.push_back(new CountdownAnalog(60));
	theCountdowns.back()->setFont(&oldenglish);
	theCountdowns.back()->setLocation(positionX[3], positionY);
	theCountdowns.back()->setFontSize(80);

	//theCountdowns.back()->setAdjustTime(1);

	auto start = std::chrono::system_clock::now();

	while (FSKEY_ESC != key)
	{
		FsPollDevice();
		key = FsInkey();

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// draw black rectangle
		glColor3d(0.0, 0.0, 0.0);
		drawRectangle(0, 0, 800, 550, true);

		// draw countdown clocks
		glColor3d(1.0, 0.0, 1.0);
		int i = 0;
		for (auto& currCount : theCountdowns) {
			drawCrosshairs(positionX[i++], positionY, 20);
			currCount->draw(start);
		}

		// reset start time if last one is done
		//if (theCountdowns.back()->isDone(start))
		//	start = std::chrono::system_clock::now();

		FsSwapBuffers();
		FsSleep(20);

	}

	// release the countdowns (which were created using "new")
	// doing it from "the back forwards" so that vector doesn't have to 
	// move things around.
	while(theCountdowns.size() > 0) {
		delete theCountdowns.back();
		theCountdowns.pop_back();
	}

}
