#include <stdio.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <array>
#include <ctime>  
#include "fssimplewindow.h"
#include "yspng.h"
#include "Countdown.h"
#include "yssimplesound.h"
#include "GraphicFont.h"

using namespace std;

// declare constants
const double PI = 3.14159265;

// declare variables
enum season { spring, summer, autumn, winter };
season theSeason;

// declare functions
void setImages(int numImages, GLuint* texId, YsRawPngDecoder* png, string* imageFileNames);
void drawWindow(GLuint* texId, YsRawPngDecoder* png);
void drawSeason(GLuint* texId, YsRawPngDecoder* png, season theSeason);
void drawCup(GLuint* texId, YsRawPngDecoder* png);
void drawClock(GLuint* texId, YsRawPngDecoder* png);
void drawCoffee(GLuint* texId, YsRawPngDecoder* png, int timeElapsed);
void drawTrampoline(GLuint* texId, YsRawPngDecoder* png);
void drawFence(GLuint* texId, YsRawPngDecoder* png);
void drawEgg(GLuint* texId, YsRawPngDecoder* png, int& wid, double& xTranslateEgg, double& angleEgg);
void drawStone(GLuint* texId, YsRawPngDecoder* png);
void drawEggShell(GLuint* texId, YsRawPngDecoder* png, int& hei, double& yTranslateEggShell);
void drawPan(GLuint* texId, YsRawPngDecoder* png);
void drawFryingEgg(GLuint* texId, YsRawPngDecoder* png);
void drawSmile(GLuint* texId, YsRawPngDecoder* png);
void drawWink(GLuint* texId, YsRawPngDecoder* png);

int main(void)
{

	/*vector<Countdown*> theCountdowns;*/
	string fileNames[] = { "optimistic.wav", "bounce.wav", "rooster.wav", "hammer.wav", "crack.wav", "frying.wav", "alarm.wav" };
	string imageFileNames[] = { "coffee_empty.png", "coffee_content.png", "chick03.png",
								"clock_empty.png", "egg02.png", "rooster02.png", "window.png",
								"spring.png", "summer.png", "autumn.png", "winter.png",
								"trampoline.png", "fence.png", "hammer.png", "stone.png",
								"eggshell.png", "pan.png", "fryingegg.png", "smile.png",
								"wink.png" };
	string* imageFileNamesPtr = imageFileNames;

	const int imageQuantity = 20;

	// Sound
	YsSoundPlayer player1, player2, player3, player4, player5, player6, player7;
	YsSoundPlayer::SoundData myWav1, myWav2, myWav3, myWav4, myWav5, myWav6, myWav7;
	enum playStyle { once, loop };

	YsRawPngDecoder png[imageQuantity];
	YsRawPngDecoder* pngPtr = png;


	FsOpenWindow(16, 16, 1400, 1050, 1);


	GLuint texId[imageQuantity];  // this will be used as texture identifiers for the PNGs
	GLuint* texIdPtr = texId;

	// create countdown object
	CountdownAnalog hourHand = CountdownAnalog(12);
	hourHand.setFontSize(80);

	// sets up the images for drawing
	setImages(imageQuantity, texIdPtr, pngPtr, imageFileNamesPtr);

	// sound playing
	if (YSOK == myWav1.LoadWav(fileNames[0].c_str())) {

		// start the player after opening window
		player1.Start();
		player1.PlayBackground(myWav1);

		// begin of timing
		auto begin = std::chrono::system_clock::now();
		
		// declare variables, initialize if appropriate
		double angleChick = 0.0;

		bool notYetCrowed = true;
		bool notYetHit = true;
		bool eggCracked = false;
		bool fryingStarted = false;
		bool alarmRang = false;
		bool timeHasStopped = false;

		double currThetaHammer, startThetaHammer = PI / 4, dTheta = PI / 12;
		double hammerLength, hammerWidth;

		double angleEgg1 = 0.0;
		double xTranslateEgg1 = 0.0;

		double angleEgg2 = 0.0;
		double xTranslateEgg2 = 0.0;

		double yTranslateEggShell = 0.0;

		double adjustTime = 1.0;

		int key = FSKEY_NULL;

		while (FSKEY_ESC != key)
		{
			FsPollDevice();

			key = FsInkey();

			int wid, hei;
			FsGetWindowSize(wid, hei);   // nice little function

			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			// in order for textures to show, you must create a projection
			glViewport(0, 0, wid, hei);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0, (float)wid - 1, (float)hei - 1, 0, -1, 1);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glColor4d(1.0, 1.0, 1.0, 1.0);   // this color will "tint" the image

			// enable texture mapping
			glEnable(GL_TEXTURE_2D);

			// turn on blending so that transparency can be applied
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// use time to determine "amount" of coffee content
			
			auto thisMoment = std::chrono::system_clock::now();
			int timeElapsed = std::chrono::duration_cast<std::chrono::seconds>
				(std::chrono::system_clock::now() - begin).count() / adjustTime;

			// draw coffee content
			if (!timeHasStopped) {
				drawCoffee(texIdPtr, pngPtr, timeElapsed);
			}
			else {
				drawCoffee(texIdPtr, pngPtr, 0);
			}

			// draw cup
			drawCup(texIdPtr, pngPtr);


			// draw clock
			drawClock(texIdPtr, pngPtr);

			if (timeElapsed >= 0 && timeElapsed < 6) {
				// draw egg
				drawEgg(texIdPtr, pngPtr, wid, xTranslateEgg1, angleEgg1);
			}

			// Chick bouncing up and down on trampoline
			else if (timeElapsed >= 6 && timeElapsed < 12) {

				// draw trampoline
				drawTrampoline(texIdPtr, pngPtr);

				//bind a texture to OpenGL primitives
				glBindTexture(GL_TEXTURE_2D, texId[2]);   // chick 
				double scale2 = 0.3;
				int xSize1 = png[2].wid * scale2;
				int ySize1 = png[2].hei * scale2;
				glBegin(GL_QUADS);

				//draw chick (at lower end of screen and mirror image)
				glTexCoord2d(1.0, 0.0);      // upper RIGHT of image
				glVertex2i(10, 400 + 100 * sin(angleChick));        // upper LEFT of rectangle on screen

				glTexCoord2d(0.0, 0.0);      // upper LEFT of image
				glVertex2i(10 + xSize1, 400 + 100 * sin(angleChick));// upper RIGHT of rectangle on screen

				glTexCoord2d(0.0, 1.0);      // lower LEFT of image
				glVertex2i(10 + xSize1, 400 + ySize1 + 100 * sin(angleChick)); // lower RIGHT of rectangle on screen

				glTexCoord2d(1.0, 1.0);       // lower RIGHT of image
				glVertex2i(10, 400 + ySize1 + 100 * sin(angleChick));     // lower LEFT of rectangle on screen

				glEnd();

				// bounce sound when at the lowest point of jumping
				if (sin(angleChick) > 0.999) { // lowest point is when sine is largest since positive y is downward

					if (YSOK == myWav2.LoadWav(fileNames[1].c_str())) {
						player2.Start();
						player2.PlayOneShot(myWav2);
					}
				}

				angleChick += 0.08;
			}

			else if (timeElapsed >= 12 && timeElapsed < 18) {
				// draw fence
				drawFence(texIdPtr, pngPtr);

				//bind a texture to OpenGL primitives
				glBindTexture(GL_TEXTURE_2D, texId[5]);   // rooster 
				double scale5 = 0.3;
				int xSize5 = png[5].wid * scale5;
				int ySize5 = png[5].hei * scale5;
				glBegin(GL_QUADS);

				//draw rooster (at lower end of screen and mirror image)
				glTexCoord2d(1.0, 0.0);      // upper RIGHT of image
				glVertex2i(10, 275);        // upper LEFT of rectangle on screen

				glTexCoord2d(0.0, 0.0);      // upper LEFT of image
				glVertex2i(10 + xSize5, 275);// upper RIGHT of rectangle on screen

				glTexCoord2d(0.0, 1.0);      // lower LEFT of image
				glVertex2i(10 + xSize5, 275 + ySize5); // lower RIGHT of rectangle on screen

				glTexCoord2d(1.0, 1.0);       // lower RIGHT of image
				glVertex2i(10, 275 + ySize5);     // lower LEFT of rectangle on screen

				glEnd();

				if (notYetCrowed) {
					if (YSOK == myWav3.LoadWav(fileNames[2].c_str())) {
						player3.Start();
						player3.PlayOneShot(myWav3);
						notYetCrowed = false;
					}
				}

			}

			else if (timeElapsed >= 18 && timeElapsed < 22) {
				// hammer hits clock

				// bind a texture to OpenGL primitives
				glBindTexture(GL_TEXTURE_2D, texId[13]);   // hammer
				double scale13 = 0.4;
				int xSize13 = png[13].wid * scale13;
				int ySize13 = png[13].hei * scale13;

				int locX = 280; // coordinates of the left upper corner
				int locY = 100;

				currThetaHammer = startThetaHammer - dTheta * (timeElapsed - 18);
				hammerLength = xSize13;
				hammerWidth = ySize13;

				double leftUpperX = locX - 0.5 * hammerWidth * (sin(currThetaHammer));
				double leftUpperY = locY - 0.5 * hammerWidth * (cos(currThetaHammer));
				double leftLowerX = locX + 0.5 * hammerWidth * (sin(currThetaHammer));
				double leftLowerY = locY + 0.5 * hammerWidth * (cos(currThetaHammer));
				double rightLowerX = leftLowerX + hammerLength * cos(currThetaHammer);
				double rightLowerY = leftLowerY - hammerLength * sin(currThetaHammer);
				double rightUpperX = leftUpperX + hammerLength * cos(currThetaHammer);
				double rightUpperY = leftUpperY - hammerLength * sin(currThetaHammer);

				glBegin(GL_QUADS);
				glTexCoord2d(0.0, 0.0);
				glVertex2d(leftUpperX, leftUpperY);

				glTexCoord2d(0.0, 1.0);
				glVertex2d(leftLowerX, leftLowerY);

				glTexCoord2d(1.0, 1.0);
				glVertex2d(rightLowerX, rightLowerY);

				glTexCoord2d(1.0, 0.0);
				glVertex2d(rightUpperX, rightUpperY);
				glEnd();
			}

			if (timeElapsed == 21 && notYetHit) {
				// hitting sound
				if (YSOK == myWav4.LoadWav(fileNames[3].c_str())) {
					player4.Start();
					player4.PlayOneShot(myWav4);
					notYetHit = false;
				}
				// time stops
				timeHasStopped = true;
			}

			// time resumes
			if (timeElapsed == 23) {
				timeHasStopped = false;
			}

			// egg rolls and crack on stone
			if (timeElapsed >= 25 && timeElapsed < 29) {
				drawStone(texIdPtr, pngPtr);
			}
			if (timeElapsed >= 25 && timeElapsed < 28) {
				drawEgg(texIdPtr, pngPtr, wid, xTranslateEgg2, angleEgg2);
			}
			// egg cracking sound
			if (timeElapsed >= 28) {
				if (!eggCracked) {
					if (YSOK == myWav5.LoadWav(fileNames[4].c_str())) {
						player5.Start();
						player5.PlayOneShot(myWav5);
						eggCracked = true;
					}
				}

			}

			// frying pan waiting once egg cracks
			if (timeElapsed >= 28) {
				drawPan(texIdPtr, pngPtr);
			}

			// broken egg shell descends into frying pan
			if (timeElapsed >= 28 && timeElapsed < 28.2) {
				drawEggShell(texIdPtr, pngPtr, hei, yTranslateEggShell);
			}
			// upon landing onto frying pan, it becomes the fried egg
			if (timeElapsed >= 29) {
				drawFryingEgg(texIdPtr, pngPtr);
				// play frying sound
				if (!fryingStarted) {
					if (YSOK == myWav6.LoadWav(fileNames[5].c_str())) {
						player6.Start();
						player6.PlayOneShot(myWav6);
						fryingStarted = true;
					}
				}

			}

			// draw empty tramoline (no chick jumping on it) when time comes
			if (timeElapsed >= 31 && timeElapsed < 37) {
				drawTrampoline(texIdPtr, pngPtr);
			}

			// draw empty fence (no rooster on it)
			if (timeElapsed >= 37 && timeElapsed <= 43) {
				drawFence(texIdPtr, pngPtr);
			}

			// alarm clock rings
			if (timeElapsed > 43 && !alarmRang) {
				if (YSOK == myWav7.LoadWav(fileNames[6].c_str())) {
					player7.Start();
					player7.PlayOneShot(myWav7);
					alarmRang = true;
				}
				// time stops
				timeHasStopped = true;
				// frying sound pauses
				player6.Pause(myWav6);

			}

			// alarm clock smiles
			if (timeElapsed >= 49 && timeElapsed <50) {
				drawSmile(texIdPtr, pngPtr);
			}

			// alarm clock winks
			if (timeElapsed >= 50 && timeElapsed < 50.5) {
				drawWink(texIdPtr, pngPtr);
			}

			// alarm clock smiles
			if (timeElapsed >= 50.5 && timeElapsed < 51.5) {
				drawSmile(texIdPtr, pngPtr);
			}

			// time resumes
			if (timeElapsed == 52) {
				timeHasStopped = false;
				// frying sound resumes
				player6.Resume(myWav6);
			}

			if (!timeHasStopped) {
				// cycle through the seasons every 3 seconds
				if (timeElapsed % 16 >= 0 && timeElapsed % 16 < 4) {
					theSeason = spring;
					//spring
				}
				else if (timeElapsed % 16 >= 4 && timeElapsed % 16 < 8) {
					theSeason = summer;

				}

				else if (timeElapsed % 16 >= 8 && timeElapsed % 16 < 12) {
					theSeason = autumn;
				}

				else {
					theSeason = winter;
				}

			}
			// draw season
			drawSeason(texIdPtr, pngPtr, theSeason);
			// window
			drawWindow(texIdPtr, pngPtr);

			// turn off blending 
			glDisable(GL_BLEND);

			// draw countdown clocks
			hourHand.setLocation(617, 342); // rotates about the clock center (trial and error)
			glColor4d(0., 0., 0., 0.);
			if (!timeHasStopped) {
				hourHand.draw(begin);
			}
			else {
				hourHand.draw(thisMoment); // set start time as this moment so that it will not "tick"
			}

			if (timeHasStopped) {
				player1.Pause(myWav1);
			}
			else {
				player1.Resume(myWav1);
			}

			FsSwapBuffers();
			FsSleep(25);

		}

		//}

		// common courtesy to close player, although destructor of player 
		// will end player when variable passes "out of scope" at end of main()
		player1.End();

	}

	return 0;
}

void drawWindow(GLuint* texId, YsRawPngDecoder* png) {
	// bind a texture to OpenGL primitives
	glBindTexture(GL_TEXTURE_2D, texId[6]);   // window
	double scale6 = 0.6;
	int xSize6 = png[6].wid * 0.4 * scale6;
	int ySize6 = png[6].hei * 0.6 * scale6;
	glBegin(GL_QUADS);

	//draw window
	glTexCoord2d(0.3, 0.1);   // these are "percentage" of the image
	glVertex2i(800, 100);        // these are actual pixel coordinates on screen

	glTexCoord2d(0.7, 0.1);
	glVertex2i(800 + xSize6, 100);

	glTexCoord2d(0.7, 0.7);
	glVertex2i(800 + xSize6, 100 + ySize6);

	glTexCoord2d(0.3, 0.7);
	glVertex2i(800, 100 + ySize6);

	glEnd();
}

void drawSeason(GLuint* texId, YsRawPngDecoder* png, season theSeason) {
	if (theSeason == spring) {
		// bind a texture to OpenGL primitives
		glBindTexture(GL_TEXTURE_2D, texId[7]);   // spring
		double scale7 = 0.6;
		int xSize7 = png[7].wid * 0.4 * scale7;
		int ySize7 = png[7].hei * 0.6 * scale7;

		glBegin(GL_QUADS);

		//draw spring
		glTexCoord2d(0.3, 0.1);   // these are "percentage" of the image
		glVertex2i(800, 100);        // these are actual pixel coordinates on screen

		glTexCoord2d(0.7, 0.1);
		glVertex2i(800 + xSize7, 100);

		glTexCoord2d(0.7, 0.7);
		glVertex2i(800 + xSize7, 100 + ySize7);

		glTexCoord2d(0.3, 0.7);
		glVertex2i(800, 100 + ySize7);

		glEnd();
	}
	if (theSeason == summer) {
		// bind a texture to OpenGL primitives
		glBindTexture(GL_TEXTURE_2D, texId[8]);   // summer
		double scale8 = 0.6;
		int xSize8 = png[8].wid * 0.4 * scale8;
		int ySize8 = png[8].hei * 0.6 * scale8;

		glBegin(GL_QUADS);

		//draw summer
		glTexCoord2d(0.3, 0.1);   // these are "percentage" of the image
		glVertex2i(800, 100);        // these are actual pixel coordinates on screen

		glTexCoord2d(0.7, 0.1);
		glVertex2i(800 + xSize8, 100);

		glTexCoord2d(0.7, 0.7);
		glVertex2i(800 + xSize8, 100 + ySize8);

		glTexCoord2d(0.3, 0.7);
		glVertex2i(800, 100 + ySize8);

		glEnd();
	}

	if (theSeason == autumn) {
		// bind a texture to OpenGL primitives
		glBindTexture(GL_TEXTURE_2D, texId[9]);   // autumn
		double scale9 = 0.6;
		int xSize9 = png[9].wid * 0.4 * scale9;
		int ySize9 = png[9].hei * 0.6 * scale9;

		glBegin(GL_QUADS);

		//draw autumn
		glTexCoord2d(0.3, 0.1);   // these are "percentage" of the image
		glVertex2i(800, 100);        // these are actual pixel coordinates on screen

		glTexCoord2d(0.7, 0.1);
		glVertex2i(800 + xSize9, 100);

		glTexCoord2d(0.7, 0.7);
		glVertex2i(800 + xSize9, 100 + ySize9);

		glTexCoord2d(0.3, 0.7);
		glVertex2i(800, 100 + ySize9);

		glEnd();
	}
	if (theSeason == winter) {
		// bind a texture to OpenGL primitives
		glBindTexture(GL_TEXTURE_2D, texId[10]);   // winter
		double scale10 = 0.6;
		int xSize10 = png[10].wid * 0.4 * scale10;
		int ySize10 = png[10].hei * 0.6 * scale10;

		glBegin(GL_QUADS);

		//draw winter
		glTexCoord2d(0.3, 0.1);   // these are "percentage" of the image
		glVertex2i(800, 100);        // these are actual pixel coordinates on screen

		glTexCoord2d(0.7, 0.1);
		glVertex2i(800 + xSize10, 100);

		glTexCoord2d(0.7, 0.7);
		glVertex2i(800 + xSize10, 100 + ySize10);

		glTexCoord2d(0.3, 0.7);
		glVertex2i(800, 100 + ySize10);

		glEnd();
	}
}

void drawCup(GLuint* texId, YsRawPngDecoder* png) {
	// bind a texture to OpenGL primitives
	glBindTexture(GL_TEXTURE_2D, texId[0]);   // coffee_empty (empty coffee cup)
	double scale0 = 0.5;
	int xSize0 = png[0].wid * scale0;
	int ySize0 = png[0].hei * scale0;

	glBegin(GL_QUADS);
	// For each vertex, assign texture coordinate before vertex coordinate.
	glTexCoord2d(0.0, 0.0);   // these are "percentage" of the image
	glVertex2i(30, -100);        // these are actual pixel coordinates on screen

	glTexCoord2d(1.0, 0.0);
	glVertex2i(30 + xSize0, -100);

	glTexCoord2d(1.0, 1.0);
	glVertex2i(30 + xSize0, -100 + ySize0);

	glTexCoord2d(0.0, 1.0);
	glVertex2i(30, -100 + ySize0);

	glEnd();
}

void drawClock(GLuint* texId, YsRawPngDecoder* png) {
	// bind a texture to OpenGL primitives
	glBindTexture(GL_TEXTURE_2D, texId[3]);   // clock
	double scale3 = 0.3;
	int xSize3 = png[3].wid * scale3; //xSize3 = 576
	int ySize3 = png[3].hei * scale3; //ySize3 = 384
	glBegin(GL_QUADS);

	//draw clock 
	glTexCoord2d(0.0, 0.0);   // these are "percentage" of the image
	glVertex2i(350, 100);        // these are actual pixel coordinates on screen

	glTexCoord2d(1.0, 0.0);
	glVertex2i(350 + xSize3, 100);

	glTexCoord2d(1.0, 1.0);
	glVertex2i(350 + xSize3, 100 + ySize3);

	glTexCoord2d(0.0, 1.0);
	glVertex2i(350, 100 + ySize3);

	glEnd();
}

void drawCoffee(GLuint* texId, YsRawPngDecoder* png, int timeElapsed) {
	// bind a texture to OpenGL primitives
	glBindTexture(GL_TEXTURE_2D, texId[1]);   // coffee_content (coffee content)

	double scale1 = 0.5;
	int xSize1 = png[1].wid * scale1;
	int ySize1 = png[1].hei * scale1;

	glBegin(GL_QUADS);
	// For each vertex, assign texture coordinate before vertex coordinate.
	glTexCoord2d(0.0, 0.0);   // these are "percentage" of the image
	glVertex2i(30, (timeElapsed * 4) % 120 - 100);        // these are actual pixel coordinates on screen

	glTexCoord2d(1.0, 0.0);
	glVertex2i(30 + xSize1, (timeElapsed * 4) % 120 - 100);

	glTexCoord2d(1.0, 1.0);
	glVertex2i(30 + xSize1, (timeElapsed * 4) % 120 + ySize1 - 100);

	glTexCoord2d(0.0, 1.0);
	glVertex2i(30, (timeElapsed * 4) % 120 + ySize1 - 100);

	glEnd();
}

void drawTrampoline(GLuint* texId, YsRawPngDecoder* png) {
	//bind a texture to OpenGL primitives
	glBindTexture(GL_TEXTURE_2D, texId[11]);   // trampoline
	double scale11 = 0.4;
	int xSize11 = png[11].wid * scale11;
	int ySize11 = png[11].hei * scale11;
	glBegin(GL_QUADS);

	//draw trampoline 
	glTexCoord2d(0.0, 0.0);
	glVertex2i(60, 500);        // upper LEFT of rectangle on screen

	glTexCoord2d(1.0, 0.0);
	glVertex2i(60 + xSize11, 500);// upper RIGHT of rectangle on screen

	glTexCoord2d(1.0, 1.0);
	glVertex2i(60 + xSize11, 500 + ySize11); // lower RIGHT of rectangle on screen

	glTexCoord2d(0.0, 1.0);
	glVertex2i(60, 500 + ySize11);     // lower LEFT of rectangle on screen

	glEnd();
}

void drawFence(GLuint* texId, YsRawPngDecoder* png) {
	//bind a texture to OpenGL primitives
	glBindTexture(GL_TEXTURE_2D, texId[12]);   // fence
	double scale12 = 0.3;
	int xSize12 = png[12].wid * scale12;
	int ySize12 = png[12].hei * scale12;
	glBegin(GL_QUADS);

	//draw fence
	glTexCoord2d(0.0, 0.0);
	glVertex2i(10, 500);        // upper LEFT of rectangle on screen

	glTexCoord2d(1.0, 0.0);
	glVertex2i(10 + xSize12, 500);// upper RIGHT of rectangle on screen

	glTexCoord2d(1.0, 1.0);
	glVertex2i(10 + xSize12, 500 + ySize12); // lower RIGHT of rectangle on screen

	glTexCoord2d(0.0, 1.0);       // lower RIGHT of image
	glVertex2i(10, 500 + ySize12);     // lower LEFT of rectangle on screen

	glEnd();
}


// Rolling and rotating egg
void drawEgg(GLuint* texId, YsRawPngDecoder* png, int& wid, double& xTranslateEgg, double& angleEgg) {

	//bind a texture to OpenGL primitives
	glBindTexture(GL_TEXTURE_2D, texId[4]);   // egg 
	double scale4 = 0.4;
	int xSize4 = png[4].wid * scale4;
	int ySize4 = png[4].hei * scale4;

	//draw egg 
	glBegin(GL_QUADS);

	glTexCoord2d(0.0, 0.0);
	glVertex2i(10 + xTranslateEgg + 150.0 * cos(angleEgg), 600 - 150.0 * sin(angleEgg));        // upper LEFT of rectangle on screen

	glTexCoord2d(1.0, 0.0);
	glVertex2i(10 + xTranslateEgg + 150.0 * cos(angleEgg + PI / 2.0), 600 - 150.0 * sin(angleEgg + PI / 2.0));// upper RIGHT of rectangle on screen

	glTexCoord2d(1.0, 1.0);
	glVertex2i(10 + xTranslateEgg + 150.0 * cos(angleEgg + PI), 600 - 150.0 * sin(angleEgg + PI)); // lower RIGHT of rectangle on screen

	glTexCoord2d(0.0, 1.0);
	glVertex2i(10 + xTranslateEgg + 150.0 * cos(angleEgg - PI / 2.0), 600 - 150.0 * sin(angleEgg - PI / 2.0));     // lower LEFT of rectangle on screen

	glEnd();

	// change the angle a bit to create spinning animation
	angleEgg -= 0.06;
	xTranslateEgg += wid / 150;
}

void drawStone(GLuint* texId, YsRawPngDecoder* png) {
	//bind a texture to OpenGL primitives
	glBindTexture(GL_TEXTURE_2D, texId[14]);   // stone
	double scale14 = 0.4;
	int xSize14 = png[14].wid * scale14;
	int ySize14 = png[14].hei * scale14;
	glBegin(GL_QUADS);

	//draw trampoline 
	glTexCoord2d(0.0, 0.0);
	glVertex2i(800, 500);        // upper LEFT of rectangle on screen

	glTexCoord2d(1.0, 0.0);
	glVertex2i(800 + xSize14, 500);// upper RIGHT of rectangle on screen

	glTexCoord2d(1.0, 1.0);
	glVertex2i(800 + xSize14, 500 + ySize14); // lower RIGHT of rectangle on screen

	glTexCoord2d(0.0, 1.0);
	glVertex2i(800, 500 + ySize14);     // lower LEFT of rectangle on screen

	glEnd();
}

void drawEggShell(GLuint* texId, YsRawPngDecoder* png, int& hei, double& yTranslateEggShell) {
	//bind a texture to OpenGL primitives
	glBindTexture(GL_TEXTURE_2D, texId[15]);   // eggshell
	double scale15 = 0.7;
	int xSize15 = png[15].wid * scale15;
	int ySize15 = png[15].hei * scale15;
	glBegin(GL_QUADS);

	//draw trampoline 
	glTexCoord2d(0.0, 0.0);
	glVertex2i(800, 600 + yTranslateEggShell);        // upper LEFT of rectangle on screen

	glTexCoord2d(1.0, 0.0);
	glVertex2i(800 + xSize15, 600 + yTranslateEggShell);// upper RIGHT of rectangle on screen

	glTexCoord2d(1.0, 1.0);
	glVertex2i(800 + xSize15, 600 + ySize15 + yTranslateEggShell); // lower RIGHT of rectangle on screen

	glTexCoord2d(0.0, 1.0);
	glVertex2i(800, 600 + ySize15 + yTranslateEggShell);     // lower LEFT of rectangle on screen

	glEnd();

	yTranslateEggShell += hei / 150;
}

void drawPan(GLuint* texId, YsRawPngDecoder* png) {
	//bind a texture to OpenGL primitives
	glBindTexture(GL_TEXTURE_2D, texId[16]);   // pan
	double scale16 = 0.4;
	int xSize16 = png[16].wid * scale16;
	int ySize16 = png[16].hei * scale16;
	glBegin(GL_QUADS);

	//draw pan
	glTexCoord2d(0.0, 0.0);
	glVertex2i(720, 700);        // upper LEFT of rectangle on screen

	glTexCoord2d(1.0, 0.0);
	glVertex2i(720 + xSize16, 700);// upper RIGHT of rectangle on screen

	glTexCoord2d(1.0, 1.0);
	glVertex2i(720 + xSize16, 700 + ySize16); // lower RIGHT of rectangle on screen

	glTexCoord2d(0.0, 1.0);
	glVertex2i(720, 700 + ySize16);     // lower LEFT of rectangle on screen

	glEnd();
}

void drawFryingEgg(GLuint* texId, YsRawPngDecoder* png) {
	//bind a texture to OpenGL primitives
	glBindTexture(GL_TEXTURE_2D, texId[17]);   // frying egg
	double scale17 = 0.4;
	int xSize17 = png[17].wid * scale17;
	int ySize17 = png[17].hei * scale17;
	glBegin(GL_QUADS);

	//draw pan
	glTexCoord2d(0.0, 0.0);
	glVertex2i(800, 800);        // upper LEFT of rectangle on screen

	glTexCoord2d(1.0, 0.0);
	glVertex2i(800 + xSize17, 800);// upper RIGHT of rectangle on screen

	glTexCoord2d(1.0, 1.0);
	glVertex2i(800 + xSize17, 800 + ySize17); // lower RIGHT of rectangle on screen

	glTexCoord2d(0.0, 1.0);
	glVertex2i(800, 800 + ySize17);     // lower LEFT of rectangle on screen

	glEnd();
}

void drawSmile(GLuint* texId, YsRawPngDecoder* png) {
	// bind a texture to OpenGL primitives
	glBindTexture(GL_TEXTURE_2D, texId[18]);   // clock smiles
	double scale18 = 0.2;
	int xSize18 = png[18].wid * scale18;
	int ySize18 = png[18].hei * scale18;
	glBegin(GL_QUADS);

	//draw clock 
	glTexCoord2d(0.0, 0.0);   // these are "percentage" of the image
	glVertex2i(500, 250);        // these are actual pixel coordinates on screen

	glTexCoord2d(1.0, 0.0);
	glVertex2i(500 + xSize18, 250);

	glTexCoord2d(1.0, 1.0);
	glVertex2i(500 + xSize18, 250 + ySize18);

	glTexCoord2d(0.0, 1.0);
	glVertex2i(500, 250 + ySize18);

	glEnd();
}

void drawWink(GLuint* texId, YsRawPngDecoder* png) {
	// bind a texture to OpenGL primitives
	glBindTexture(GL_TEXTURE_2D, texId[19]);   // clock winks
	double scale19 = 0.2;
	int xSize19 = png[19].wid * scale19;
	int ySize19 = png[19].hei * scale19;
	glBegin(GL_QUADS);

	//draw clock 
	glTexCoord2d(0.0, 0.0);   // these are "percentage" of the image
	glVertex2i(500, 250);        // these are actual pixel coordinates on screen

	glTexCoord2d(1.0, 0.0);
	glVertex2i(500 + xSize19, 250);

	glTexCoord2d(1.0, 1.0);
	glVertex2i(500 + xSize19, 250 + ySize19);

	glTexCoord2d(0.0, 1.0);
	glVertex2i(500, 250 + ySize19);

	glEnd();
}

void setImages(int numImages, GLuint* texId, YsRawPngDecoder* png, string* imageFileNames) {
	for (int i = 0; i < numImages; i++) {
		png[i].Decode(imageFileNames[i].c_str());
		glGenTextures(1, &texId[i]);  // Reserve one texture identifier
		glBindTexture(GL_TEXTURE_2D, texId[i]);  // Making the texture identifier current (or bring it to the deck)

		// set up parameters for the current texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D
		(GL_TEXTURE_2D,
			0,    // Level of detail
			GL_RGBA,       // the "A" in RGBA will include the transparency
			png[i].wid,    // the chick width and height
			png[i].hei,
			0,    // Border width, but not supported and needs to be 0.
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			png[i].rgba);
	}

}