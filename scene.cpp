#include "gameresource.h"
#include "scene.h"
#include <QPainter>
#include <deque>
#define SPEED 0.08f

Scene::Scene(QObject *parent) : QGraphicsScene (parent)
{
	totAlpha = 1.0f;
	camera = new Camera(this);
	dq.push_back(-2.0);
	dq.push_back(-14.0f);
	dq.push_back(-26.0);

	tree_dq.push_back(-6.0f);
	tree_dq.push_back(-18.0f);

	tree2_dq.push_back(-2.0f);
	tree2_dq.push_back(-14.0f);
}

Scene::~Scene()
{
	delete bg_VBO;
	delete bg_VAO;
	delete bg_texture;
	delete bg_vs;
	delete bg_program;

	delete tree_texture;
	delete star_texture;
}
void Scene::drawBackground(QPainter *painter, const QRectF &rect){
//	qDebug() << "paint";
	static int lock = false;
	if(!lock) {
		init();
		lock = true;
	}else{
		painter->beginNativePainting();
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glEnable(GL_DEPTH_TEST);
//		glDepthMask(GL_FALSE);

		bg_VAO->bind();
		bg_program->bind();
		bg_texture->bind(0);
		for(float s:dq) {
			QMatrix4x4 pr, matrix;
			matrix.setToIdentity();
			pr.setToIdentity();
			pr.perspective(45.0, 487.0f / 557.0f, 0.1f, 50.0f);
			matrix.translate(0, 0.0, s);
			matrix.rotate(-90, 1.0, 0, 0.0);
			matrix.scale(6.0f, 6.0f);
			camera->setPos(QVector3D(0, 1.2f, 6.0f));
			camera->setPitch(0);
			bg_program->setUniformValue("alpha", totAlpha);
			bg_program->setUniformValue("projection", pr * camera->getMatrix() * matrix);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		for(float &s:dq) s += SPEED;
		if(dq.front() >= 9.5f) dq.pop_front();
		if(dq.back() <= -14.0f && dq.back() + SPEED > -14.0f) dq.push_back(-26.0f);
		bg_texture->release();
		tree_texture->bind(0);

		for(float s:tree_dq) {
			QMatrix4x4 pr, matrix;
			matrix.setToIdentity();
			pr.setToIdentity();
			pr.perspective(45.0, 487.0f / 557.0f, 0.1f, 50.0f);
			matrix.translate(-3.8f, 2.0f, s);
			matrix.scale(2.8f, 2.8f);
			camera->setPos(QVector3D(0, 1.5f, 6.0f));
			camera->setPitch(0);
			bg_program->setUniformValue("alpha", totAlpha);
			bg_program->setUniformValue("projection", pr * camera->getMatrix() * matrix);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		for(float &s:tree_dq) s += SPEED * 1.5f;
		if(tree_dq.front() >= 8.5f) tree_dq.pop_front();
		if(tree_dq.back() <= -10.0f && tree_dq.back() + SPEED * 1.5f > -10.0f) tree_dq.push_back(-18.0f);

		for(float s:tree2_dq) {
			QMatrix4x4 pr, matrix;
			matrix.setToIdentity();
			pr.setToIdentity();
			pr.perspective(45.0, 487.0f / 557.0f, 0.1f, 50.0f);
			matrix.translate(3.8f, 2.0f, s);
			matrix.scale(2.8f, 2.8f);
			camera->setPos(QVector3D(0, 1.5f, 6.0f));
			camera->setPitch(0);
			bg_program->setUniformValue("alpha", totAlpha);
			bg_program->setUniformValue("projection", pr * camera->getMatrix() * matrix);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		for(float &s:tree2_dq) s += SPEED * 1.5f;
		if(tree2_dq.front() >= 8.5f) tree2_dq.pop_front();
		if(tree2_dq.back() <= -6.0f && tree2_dq.back() + SPEED * 1.5f > -6.0f) tree2_dq.push_back(-14.0f);

		tree_texture->release();
		star_texture->bind(0);
		QMatrix4x4 matrix;
		matrix.setToIdentity();
		bg_program->setUniformValue("projection", matrix);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		star_texture->release();
		bg_VAO->release();
		bg_program->release();
		bg_texture->release();

		painter->endNativePainting();
	}
}

void Scene::init()
{
	initializeOpenGLFunctions();
	glViewport(0, 0, 487, 557);

	float op1[] = {//背景渲染
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		1, 2, 3
	};
	bg_VAO = new QOpenGLVertexArrayObject;
	bg_VAO->create();
	bg_VAO->bind();

	bg_VBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	bg_VBO->create();
	bg_VBO->bind();
	bg_VBO->allocate(op1, sizeof(op1));


	bg_IBO = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	bg_IBO->create();
	bg_IBO->bind();
	bg_IBO->allocate(indices, sizeof(indices));

	QImage image;
	static_cast<GameResourcePNGData*>(GameResource::getInstance()->getData(MAIN_GAME_BG_PNG))->loadData(image);
	bg_texture = new QOpenGLTexture(image.mirrored(false, true));

	static_cast<GameResourcePNGData*>(GameResource::getInstance()->getData(TREE_PNG))->loadData(image);
	tree_texture = new QOpenGLTexture(image.mirrored(false, true));

	static_cast<GameResourcePNGData*>(GameResource::getInstance()->getData(MAIN_GAME_STAR_PNG))->loadData(image);
	star_texture = new QOpenGLTexture(image.mirrored(false, true));
	star_texture->bind();

	bg_vs = new QOpenGLShader(QOpenGLShader::Vertex);
	bg_vs->compileSourceCode("#version 330 core\n"
							 "layout (location = 0) in vec3 aPos;\n"
							 "layout (location = 1) in vec2 aTexCoord;\n"
							 "out vec2 TexCoord;\n"
							 "out vec2 fog;\n"
							 "uniform mat4 projection;\n"
							 "void main()\n"
							 "{\n"
							 "gl_Position = projection * vec4(aPos, 1.0);\n"
							 "fog.x=gl_Position.z/25.0;\n"
							 "fog.y=0;\n"
							 "TexCoord = aTexCoord;\n"
							 "}");

	bg_fs = new QOpenGLShader(QOpenGLShader::Fragment);
	bg_fs->compileSourceCode("#version 330 core\n"
							 "out vec4 FragColor;\n"
							 "in vec2 TexCoord;\n"
							 "in vec2 fog;\n"

							 "uniform sampler2D ourTexture;\n"
							 "uniform vec2 alpha;\n"
							 "uniform sampler2D texture2;\n"

							 "void main()\n"
							 "{\n"
							 "FragColor = mix(texture(texture2, TexCoord),texture(ourTexture, TexCoord),alpha.x);\n"
							 "FragColor = mix(FragColor,vec4(0.0,0.0,0.0,0.0),fog.x);\n"
							 "}");

	bg_program = new QOpenGLShaderProgram;
	bg_program->addShader(bg_vs);
	bg_program->addShader(bg_fs);
	bg_program->link();
	bg_program->bind();

	bg_program->enableAttributeArray(bg_program->attributeLocation("aPos"));
	bg_program->setAttributeBuffer(bg_program->attributeLocation("aPos"), GL_FLOAT, 0, 3, sizeof(float) * 5);

	bg_program->enableAttributeArray(bg_program->attributeLocation("aTexCoord"));
	bg_program->setAttributeBuffer(bg_program->attributeLocation("aTexCoord"), GL_FLOAT, 3 * sizeof(float), 2, sizeof(float) * 5);

	bg_program->setUniformValue("ourTexture", 0);
	bg_program->setUniformValue("texture2", 1);
	bg_program->setUniformValue("alpha", totAlpha, 0.0f);


	bg_program->release();
	bg_VAO->release();
	bg_VBO->release();
	bg_IBO->release();

}