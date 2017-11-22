/*
	egpNetPlaygroundGameStateDrawable.cpp
	By Dan Buckstein

	Test game state with rendering.

	Additional contributions: 
*/

#include "egpNet/states/egpNetPlaygroundGameStateDrawable.h"


#include <GL/glew.h>


egpNetPlaygroundGameStateDrawable::egpNetPlaygroundGameStateDrawable(int ownerID)
	: egpNetPlaygroundGameState(ownerID)
{

}


void egpNetPlaygroundGameStateDrawable::ClearBuffers()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


int egpNetPlaygroundGameStateDrawable::RenderState(int canvasWidth, int canvasHeight, int canvasPosX, int canvasPosY) const
{
	ClearBuffers();

	// set viewport and projection
	// ****USING IMMEDIATE MODE FOR TESTING ONLY
	glViewport(0, 0, canvasWidth, canvasHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-0.5*canvasWidth, 0.5*canvasWidth, -0.5*canvasHeight, 0.5*canvasHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// draw agents
	glBegin(GL_POINTS);
	for (unsigned int i = 0; i < objLimit_agent; ++i)
	{
		// color
		if (m_data->m_agentStatus[i].ownerID == GetOwnerID())
		{
			glColor3f(0.0f, 1.0f, 1.0f);
			glVertex2f(m_data->m_agent[i].posX, m_data->m_agent[i].posY);
		}
		else if (m_data->m_agentStatus[i].flags & objFlag_active)
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex2f(m_data->m_agent[i].posX, m_data->m_agent[i].posY);
		}
	}

	for (unsigned int i = 0; i < objLimit_ball; ++i)
	{
		// color
		glColor3f(0.5f * i, 1.0f, 1.0f - (i * 0.5f)); //im colorblind so i really gotta mess with this, geez (-rob)
		glVertex2f(m_data->m_balls[i].posX, m_data->m_balls[i].posY);
	}

	glEnd();


	// test draw command: change background color
//	float c = (float)(m_data->m_t);
//	glClearColor(c, c, c, 1.0f);

	// do nothing
	return 0;
}