/*	Draws squares.
 *	Thats it.
 */
#include <GL/glut.h>

static void drawSquare(GLfloat size)
{
	if(size <= 0.0)
	{
		size = 0.001f;
	}

	glColor3f(1.0, 0.0, 0.0);

	//Front
	glBegin(GL_POLYGON);
		glVertex3f(-0.5f * size, -0.5f * size, 0.5f * size);
		glVertex3f(0.5f * size, -0.5f * size, 0.5f * size);
		glVertex3f(0.5f * size, 0.5f * size, 0.5f * size);
		glVertex3f(-0.5f * size, 0.5f * size, 0.5f * size);
	glEnd();

	//Back
	glBegin(GL_POLYGON);
		glVertex3f(-0.5f * size, -0.5f * size, -0.5f * size);
		glVertex3f(0.5f * size, -0.5 * size, -0.5f * size);
		glVertex3f(0.5f * size, 0.5f * size, -0.5f * size);
		glVertex3f(-0.5f * size, 0.5f * size, -0.5f * size);
	glEnd();

	glColor3f(0.0, 1.0, 1.0);

	//Right
	glBegin(GL_POLYGON);
		glVertex3f(0.5f * size, -0.5f * size, 0.5f * size);
		glVertex3f(0.5f * size, -0.5f * size, -0.5f * size);
		glVertex3f(0.5f * size, 0.5f * size, -0.5f * size);
		glVertex3f(0.5f * size, 0.5f * size, 0.5f * size);
	glEnd();

	//Left
	glBegin(GL_POLYGON);
		glVertex3f(-0.5f * size, -0.5f * size, 0.5f * size);
		glVertex3f(-0.5f * size, -0.5f * size, -0.5f * size);
		glVertex3f(-0.5f * size, 0.5f * size, -0.5f * size);
		glVertex3f(-0.5f * size, 0.5f * size, 0.5f * size);
	glEnd();

	glColor3f(0.0, 0.0, 1.0);

	//Top
	glBegin(GL_POLYGON);
		glVertex3f(-0.5f * size, 0.5f * size, 0.5f * size);
		glVertex3f(0.5f * size, 0.5f * size, 0.5f * size);
		glVertex3f(0.5f * size, 0.5f * size, -0.5f * size);
		glVertex3f(-0.5f * size, 0.5f * size, -0.5f * size);
	glEnd();

	//Bottom
	glBegin(GL_POLYGON);
		glVertex3f(-0.5f * size, -0.5f * size, 0.5f * size);
		glVertex3f(0.5f * size, -0.5f * size, 0.5f * size);
		glVertex3f(0.5f * size, -0.5f * size, -0.5f * size);
		glVertex3f(-0.5f * size, -0.5f * size, -0.5f * size);
	glEnd();
}