#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define width 1000
#define height 800
#define h_vertex 0.5f


using namespace std;

float hexa[] =
{   //����ü
	//����
	-h_vertex, h_vertex, h_vertex,
	h_vertex, h_vertex, h_vertex,
	-h_vertex, h_vertex, -h_vertex,

	h_vertex, h_vertex, h_vertex,
	h_vertex, h_vertex, -h_vertex,
	-h_vertex, h_vertex, -h_vertex,
	//����
	-h_vertex, -h_vertex, h_vertex,
	h_vertex, -h_vertex, h_vertex,
	-h_vertex, h_vertex, h_vertex,

	h_vertex, -h_vertex, h_vertex,
	h_vertex, h_vertex, h_vertex,
	-h_vertex, h_vertex, h_vertex,

	//�Ʒ���
	-h_vertex, -h_vertex, h_vertex,
	-h_vertex, -h_vertex, -h_vertex,
	h_vertex, -h_vertex, -h_vertex,

	-h_vertex, -h_vertex, h_vertex,
	h_vertex, -h_vertex, -h_vertex,
	h_vertex, -h_vertex, h_vertex,

	//�޸�
	h_vertex, -h_vertex, -h_vertex,
	-h_vertex, -h_vertex, -h_vertex,
	h_vertex, h_vertex, -h_vertex,

	-h_vertex, -h_vertex, -h_vertex,
	-h_vertex, h_vertex, -h_vertex,
	h_vertex, h_vertex, -h_vertex,


	//������
	-h_vertex, -h_vertex, -h_vertex,
	-h_vertex, h_vertex, h_vertex,
	-h_vertex, h_vertex, -h_vertex,

	-h_vertex, -h_vertex, -h_vertex,
	-h_vertex, -h_vertex, h_vertex,
	-h_vertex, h_vertex, h_vertex,

	//������
   h_vertex, -h_vertex, h_vertex,
   h_vertex, -h_vertex, -h_vertex,
   h_vertex, h_vertex, h_vertex,

   h_vertex, -h_vertex, -h_vertex,
   h_vertex, h_vertex, -h_vertex,
   h_vertex, h_vertex, h_vertex
};

float bottom[] =
{
	1, 0, -1,
	-1, 0, 1,
	1, 0, 1,
	1, 0, -1,
	-1, 0, -1,
	-1, 0, 1
};

GLvoid drawScene();
GLvoid Reshape(int, int);
GLvoid KeyBoard(unsigned char, int, int);
GLvoid Setting();

GLUquadricObj* qobj;
GLuint s_program;
GLuint VAO[3], VBO[6];
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;
GLenum obj_type{ GL_FILL };
GLint g_window_w, g_window_h;

typedef struct object {
	float x_rotate{}, y_rotate{}, z_rotate{};
	float x_rotate_aoc{}, y_rotate_aoc{}, z_rotate_aoc{};
	float ey_rotate_aoc{};
	float x_trans{}, y_trans{}, z_trans{};
	float x_trans_aoc{}, y_trans_aoc{}, z_trans_aoc{};
	float ey_trans_aoc{};
	float x_scale{}, y_scale{}, z_scale{};
}O;

typedef struct function {
	bool cull{ false };
	bool y_rotate{ false };
	bool x_is_trans{ false }, y_is_trans{ false }, z_is_trans{ false };
	bool x_is_reverse_trans{ false }, y_is_reverse_trans{ false }, z_is_reverse_trans{ false };
	bool x_is_rotate{ false }, y_is_rotate{ false }, z_is_rotate{ false };
	bool x_is_reverse_rotate{ false }, y_is_reverse_rotate{ false }, z_is_reverse_rotate{ false };
	bool start_timer{ true };
	bool crane_front_walk{ false };
	bool crane_back_walk{ false };
	bool crane_left_turn{ false };
	bool crane_right_turn{ false };
	bool crane_arm_move{ false };
	bool crane_arm_reverse_move{ false };
	bool crane_arm_spin{ false };
	bool crane_arm_Spin{ false };
	bool crane_arm_one{ false };
	bool crane_arm_two{ false };
}F;

O s, temp, camera;
O l;
F h_f, temp_f;
int count{};
bool change_type{ false };
bool pers{ true };


GLchar* filetobuf(const GLchar* file)
{
	FILE* fptr;
	long length;
	GLchar* buf;

	fopen_s(&fptr, file, "rb");
	if (!fptr)
		return NULL;

	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (GLchar*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;

	return buf;
}

void make_vertexShaders()
{
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
}

void make_fragmentShader()
{
	GLchar* fragmentSource;
	fragmentSource = filetobuf("fragment.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

}

void InitShader()
{
	make_vertexShaders();
	make_fragmentShader();

	s_program = glCreateProgram();

	glAttachShader(s_program, vertexShader);
	glAttachShader(s_program, fragmentShader);
	glLinkProgram(s_program);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR :  fragment Shader Fail Compile \n" << errorLog << endl;
		exit(-1);
	}

	else
		cout << "good";
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(s_program);
}

GLvoid InitBuffer()
{
	glGenVertexArrays(3, VAO);

	//->line
	glBindVertexArray(VAO[0]);
	glGenBuffers(2, &VBO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hexa), hexa, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO[1]);
	glGenBuffers(2, &VBO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bottom), bottom, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

void drawScene()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //���� üũ (�ø�)
	glUseProgram(s_program);

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -0.3f);			//��ġ
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);	//�ٶ󺸴� ����
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);			//ī�޶� ����
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

	view = glm::rotate(view, glm::radians(-30.f), glm::vec3(1.0f, 0.0f, 0.0f));

	view = glm::rotate(view, glm::radians(camera.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));

	view = glm::translate(view, glm::vec3(camera.x_trans, camera.y_trans, camera.z_trans));


	unsigned int viewLocation = glGetUniformLocation(s_program, "view");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	if (!pers) {
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -100.0f, 100.0f);
		unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
	else {
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
		projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));
		unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}

	glm::mat4 Bottom = glm::mat4(1.0f);
	Bottom = glm::scale(Bottom, glm::vec3(3.0f, 0.0f, 3.0f));

	unsigned int StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Bottom));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	int objColorLocation = glGetUniformLocation(s_program, "objectColor");
	unsigned isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.0f, 1.0f, 1.0f, 1.0);
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//��ü
	glm::mat4 Crane = glm::mat4(1.0f);
	Crane = glm::translate(Crane, glm::vec3(0.f, 0.f, s.z_trans_aoc));
	Crane = glm::translate(Crane, glm::vec3(s.x_trans_aoc, 0.f, 0.f));
	Crane = glm::scale(Crane, glm::vec3(1.0f, 0.5f, 1.0f));
	Crane = glm::translate(Crane, glm::vec3(0.f, 0.5f, 0.f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Crane));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 1.0f, 0.0f, 0.0f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);



	//���
	Crane = glm::mat4(1.0f);
	Crane = glm::translate(Crane, glm::vec3(0.f, 0.f, s.z_trans_aoc));
	Crane = glm::translate(Crane, glm::vec3(s.x_trans_aoc, 0.f, 0.f));
	Crane = glm::rotate(Crane, glm::radians(s.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));
	Crane = glm::translate(Crane, glm::vec3(0.0f, 0.6f, 0.0f));
	Crane = glm::scale(Crane, glm::vec3(0.5f, 0.15f, 0.5f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Crane));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.5f, 0.3f, 0.5f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//����1
	Crane = glm::mat4(1.0f);
	Crane = glm::translate(Crane, glm::vec3(0.f, 0.f, s.z_trans_aoc));
	Crane = glm::translate(Crane, glm::vec3(s.x_trans_aoc, 0.f, 0.f));
	Crane = glm::rotate(Crane, glm::radians(s.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));
	Crane = glm::translate(Crane, glm::vec3(0.1f, 0.5f + 0.5f * 0.1f, 0));
	Crane = glm::rotate(Crane, glm::radians(-s.z_rotate_aoc), glm::vec3(0.0f, 0.0f, 1.0f));
	Crane = glm::translate(Crane, glm::vec3(-0.1f, -(0.5f + 0.5f * 0.1f), 0));
	Crane = glm::rotate(Crane, glm::radians(-s.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));
	Crane = glm::translate(Crane, glm::vec3(-s.x_trans_aoc, 0.f, 0.f));
	Crane = glm::translate(Crane, glm::vec3(0.f, 0.f, -s.z_trans_aoc));

	Crane = glm::translate(Crane, glm::vec3(0.f, 0.f, s.z_trans_aoc));
	Crane = glm::translate(Crane, glm::vec3(s.x_trans_aoc, 0.f, 0.f));

	Crane = glm::rotate(Crane, glm::radians(s.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));
	
	Crane = glm::translate(Crane, glm::vec3(0.1f, 0.5f + 0.5f * 0.1f, 0));
	Crane = glm::scale(Crane, glm::vec3(0.2f, 1.0f, 0.2f));
	Crane = glm::translate(Crane, glm::vec3(0.0f, 0.5f, 0.0f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Crane));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.0f, 0.0f, 1.0f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//����2
	Crane = glm::mat4(1.0f);
	Crane = glm::translate(Crane, glm::vec3(0.f, 0.f, s.z_trans_aoc));
	Crane = glm::translate(Crane, glm::vec3(s.x_trans_aoc, 0.f, 0.f));
	Crane = glm::rotate(Crane, glm::radians(s.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));
	Crane = glm::translate(Crane, glm::vec3(-0.1f, 0.5f + 0.5f * 0.1f, 0));
	Crane = glm::rotate(Crane, glm::radians(s.z_rotate_aoc), glm::vec3(0.0f, 0.0f, 1.0f));
	Crane = glm::translate(Crane, glm::vec3(0.1f, -(0.5f + 0.5f * 0.1f), 0));
	Crane = glm::rotate(Crane, glm::radians(-s.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));
	Crane = glm::translate(Crane, glm::vec3(-s.x_trans_aoc, 0.f, 0.f));
	Crane = glm::translate(Crane, glm::vec3(0.f, 0.f, -s.z_trans_aoc));
	Crane = glm::translate(Crane, glm::vec3(0.f, 0.f, s.z_trans_aoc));
	Crane = glm::translate(Crane, glm::vec3(s.x_trans_aoc, 0.f, 0.f));
	Crane = glm::rotate(Crane, glm::radians(s.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));
	Crane = glm::translate(Crane, glm::vec3(-0.1f, 0.5f + 0.5f * 0.1f, 0));
	Crane = glm::scale(Crane, glm::vec3(0.2f, 1.0f, 0.2f));
	Crane = glm::translate(Crane, glm::vec3(0.0f, 0.5f, 0.0f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Crane));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 0.0f, 1.0f, 0.0f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//��ü �ٴ�(����)
	Crane = glm::mat4(1.0f);
	Crane = glm::translate(Crane, glm::vec3(0.f, 0.f, s.z_trans_aoc));
	Crane = glm::translate(Crane, glm::vec3(s.x_trans_aoc, 0.f, 0.f));
	Crane = glm::rotate(Crane, glm::radians(s.ey_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));
	Crane = glm::translate(Crane, glm::vec3(s.ey_trans_aoc, 0.0f, 0.f));
	Crane = glm::translate(Crane, glm::vec3(0.3f, 0.1f, -0.3f));
	Crane = glm::scale(Crane, glm::vec3(0.2f, 0.2f, 1.5f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Crane));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 1.0f, 0.3f, 0.7f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//��ü �ٴ�2
	Crane = glm::mat4(1.0f);
	Crane = glm::translate(Crane, glm::vec3(0.f, 0.f, s.z_trans_aoc));
	Crane = glm::translate(Crane, glm::vec3(s.x_trans_aoc, 0.f, 0.f));
	Crane = glm::rotate(Crane, glm::radians(-s.ey_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));
	Crane = glm::translate(Crane, glm::vec3(-s.ey_trans_aoc, 0.0f, 0.f));
	Crane = glm::translate(Crane, glm::vec3(-0.3f, 0.1f, -0.3f));
	Crane = glm::scale(Crane, glm::vec3(0.2f, 0.2f, 1.5f));
	StransformLocation = glGetUniformLocation(s_program, "transform");
	glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Crane));
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, obj_type);
	objColorLocation = glGetUniformLocation(s_program, "objectColor");
	isCheck = glGetUniformLocation(s_program, "isCheck");
	glUniform1f(isCheck, false);
	glUniform4f(objColorLocation, 1.0f, 0.3f, 0.7f, 1.0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	

	


	glutSwapBuffers();
}
void Reshape(int w, int h) {

	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);

}

GLvoid KeyBoard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'b':
		h_f.crane_front_walk = !h_f.crane_front_walk;
		break;
	case 'B':
		h_f.crane_back_walk = !h_f.crane_back_walk;
		break;
	case 'm':
		h_f.crane_left_turn = !h_f.crane_left_turn;
		break;
	case 'M':
		h_f.crane_right_turn = !h_f.crane_right_turn;
		break;
	case 't':
		h_f.crane_arm_move = !h_f.crane_arm_move;
		break;
	case 'T':
		h_f.crane_arm_reverse_move = !h_f.crane_arm_reverse_move;
		break;
	case 'f':
		h_f.crane_arm_spin = !h_f.crane_arm_spin;
		break;
	case 'F':
		h_f.crane_arm_Spin = !h_f.crane_arm_Spin;
		break;
	case 'e':
		h_f.crane_arm_one = !h_f.crane_arm_one;
		break;
	case 'E':
		h_f.crane_arm_two = !h_f.crane_arm_two;
		break;
	case 'c':
		memcpy(&s, &temp, sizeof(s));
		memcpy(&camera, &temp, sizeof(camera));
		memcpy(&h_f, &temp_f, sizeof(h_f));
		break;
	case 's':
		h_f.start_timer = !h_f.start_timer;
		break;
	case 'z':
		h_f.z_is_trans = !h_f.z_is_trans;
		break;
	case 'Z':
		h_f.z_is_reverse_trans = !h_f.z_is_reverse_trans;
		break;
	case 'x':
		h_f.x_is_trans = !h_f.x_is_trans;
		break;
	case 'X':
		h_f.x_is_reverse_trans = !h_f.x_is_reverse_trans;
		break;
	case 'r':
		h_f.y_is_rotate = !h_f.y_is_rotate;
		break;
	case 'R':
		h_f.y_is_reverse_rotate = !h_f.y_is_reverse_rotate;
		break;
	case 'q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}

GLvoid Timer(int value)
{
	if (h_f.start_timer) {
		if (h_f.crane_front_walk) {
			if (s.x_trans_aoc > -2.5)
				s.x_trans_aoc -= 0.01f;
		}
		if (h_f.crane_back_walk) {
			if (s.x_trans_aoc < 2.5)
				s.x_trans_aoc += 0.01f;
		}

		if (h_f.crane_left_turn)
			s.y_rotate_aoc++;
		if (h_f.crane_right_turn)
			s.y_rotate_aoc--;

		if (h_f.crane_arm_spin)
			s.ey_rotate_aoc++;
		if (h_f.crane_arm_Spin)
			s.ey_rotate_aoc--;

		if (h_f.crane_arm_one)
			if (s.ey_trans_aoc > -0.00000001f)
				s.ey_trans_aoc -= 0.3f;
		if (h_f.crane_arm_two)
			if (s.ey_trans_aoc < -0.00000001f)
				s.ey_trans_aoc += 0.3f;


		if (h_f.crane_arm_move) {
			// "����1"�� ���
			if (s.z_rotate_aoc >= 90) 
				s.z_rotate_aoc--;
			else if (s.z_rotate_aoc <= -90) 
				s.z_rotate_aoc++;
			else 
				s.z_rotate_aoc++;
		}

		// "����2"�� ���
		if (h_f.crane_arm_move) {
			if (s.z_rotate_aoc >= 90) 
				s.z_rotate_aoc--;
			else if (s.z_rotate_aoc <= -90) 
				s.z_rotate_aoc++;
				
		}
		if (h_f.crane_arm_reverse_move) {
			if (s.z_rotate_aoc > -90)
				s.z_rotate_aoc--;
			else if (s.z_rotate_aoc <= -90)
				s.z_rotate_aoc++;
		}

		if (h_f.z_is_trans)
			camera.z_trans += 0.01;
		if (h_f.z_is_reverse_trans)
			camera.z_trans -= 0.01;

		if (h_f.x_is_trans)
			camera.x_trans += 0.01;
		if (h_f.x_is_reverse_trans)
			camera.x_trans -= 0.01;

		if (h_f.y_is_rotate)
			camera.y_rotate_aoc += 0.1;
		if (h_f.y_is_reverse_rotate)
			camera.y_rotate_aoc -= 0.1;

	}

	glutPostRedisplay();
	glutTimerFunc(5, Timer, 1);
}

GLvoid Motion(int x, int y)
{
	glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y)
{
	glutPostRedisplay();
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("C.G_NO.20");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cerr << "NOT INIT" << endl;
	}
	else
		cout << "INIT"<<endl;
		cout << "b/B ũ���� ��ü�� x�� �������� ��/�� �̵�" << endl;
		cout << "m/M ũ���� �߾��� y�� �������� ��/�� ȸ��" << endl;
		cout << "f/F ������ y�� �������� ��/�� ȸ��" << endl;
		cout << "e/E ������ �̵��Ͽ� �ϳ��� �ȴ�/�ٽó��������" << endl;  // �̰� �ǰ� ������ �� �̻�..;;;;
		cout << "t/T ũ���� ���� -90~90�� ���̷� z���� �������� ȸ��" << endl; // �̰� �� �̻��ϱ��� ���� �����ؾ���;;;
		cout << "z/Z ī�޶� z������ ��/�� �������� �̵�" << endl;
		cout << "x/X ī�޶� x������ ��/�� �������� �̵�" << endl; 
		cout << "r/R ȭ���� �߽��� y�࿡ ���Ͽ� ī�޶� ȸ��(������ �߽����� ����)" << endl;
		cout << "a/A �˻������ �����??!!!??!!!?!!!" << endl;  // �̰� �ؾ���
		cout << "s/S ��� ������ ���߱�(�ٽ� ������ �ٽ� �� �۵�����)" << endl; 
		cout << "c/C ��� �������� �ʱ�ȭ(�ʱ� ���·Τ���)" << endl;
		cout << "q ���α׷� ����" << endl;

	InitShader();
	InitBuffer();
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(KeyBoard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutReshapeFunc(Reshape);
	glutTimerFunc(5, Timer, 1);
	glutMainLoop();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
