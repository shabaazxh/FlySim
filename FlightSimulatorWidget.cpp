///////////////////////////////////////////////////
//
//	Hamish Carr
//	October, 2023
//
//	------------------------
//	FlightSimulatorWidget.h
//	------------------------
//	
//	The main widget
//	
///////////////////////////////////////////////////

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "FlightSimulatorWidget.h"

// constructor
FlightSimulatorWidget::FlightSimulatorWidget(QWidget *parent, SceneModel *TheScene)
	: _GEOMETRIC_WIDGET_PARENT_CLASS(parent),
	theScene(TheScene)
	{ // constructor
	// we want to create a timer for forcing animation
	animationTimer = new QTimer(this);
	// connect it to the desired slot
	connect(animationTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
	// set the timer to fire 60 times a second
	animationTimer->start(16.7);
	} // constructor

// destructor
FlightSimulatorWidget::~FlightSimulatorWidget()
	{ // destructor
	// nothing yet
	} // destructor																	

// called when OpenGL context is set up
void FlightSimulatorWidget::initializeGL()
	{ // FlightSimulatorWidget::initializeGL()
	} // FlightSimulatorWidget::initializeGL()

// called every time the widget is resized
void FlightSimulatorWidget::resizeGL(int w, int h)
	{ // FlightSimulatorWidget::resizeGL()
	// reset the viewport
	glViewport(0, 0, w, h);
	
	// set projection matrix based on zoom & window size
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// compute the aspect ratio of the widget
	float aspectRatio = (float) w / (float) h;
	
	// we want a 90 degree vertical field of view, as wide as the window allows
	// and we want to see from just in front of us to 100km away
	gluPerspective(90.0, aspectRatio, 1, 100000);

	// set model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	} // FlightSimulatorWidget::resizeGL()
	
// called every time the widget needs painting
void FlightSimulatorWidget::paintGL()
	{ // FlightSimulatorWidget::paintGL()
	// call the scene to render itself
	theScene->Render();
	} // FlightSimulatorWidget::paintGL()

// called when a key is pressed
void FlightSimulatorWidget::keyPressEvent(QKeyEvent *event)
	{ // keyPressEvent()
	// just do a big switch statement
	switch (event->key())
		{ // end of key switch
		// we will use the official QT codes, even though they're mostly just ASCII
		// this means we don't distinguish between upper & lower case
		// case Qt::Key_W:
		// 	theScene->m_player->Forward();
		// 	break;
		case Qt::Key_D:
			theScene->m_player->Right();
			break;
		case Qt::Key_W:
			theScene->m_player->Left();
			break;
		case Qt::Key_S:
			theScene->m_player->PitchDown();
			break;
		case Qt::Key_A:
			theScene->m_player->PitchUp();
			break;
		case Qt::Key_Q:
			theScene->m_player->RollLeft();
			break;
		case Qt::Key_E:
			theScene->m_player->RollRight();
			break;
		case Qt::Key_Plus:
			theScene->m_player->IncreaseSpeed();
			break;
		case Qt::Key_Minus:
			theScene->m_player->DecreaseSpeed();
			break;
		case Qt::Key_V:
			theScene->SwitchCamera();
			break;
		case Qt::Key_X:
			exit(0);
			break;
		default:
			break;
		} // end of key switch
	// always update to force the animation to continue
	_GL_WIDGET_UPDATE_CALL();
	
	} // keyPressEvent()

void FlightSimulatorWidget::nextFrame()
	{ // nextFrame()
	// each time this gets called, we will update the plane's position
	theScene->Update();
	// now force an update
	update();
	} // nextFrame()

