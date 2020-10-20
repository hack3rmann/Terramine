#include "Sprite.h"
#include "GUI.h"
#include "../Window.h"

Sprite::Sprite() {
	int attrs[] = { 2, 2, 4, 0 };
	buffer = new float[GUI_VERTEX_SIZE * 6];
	mesh = new Mesh(buffer, 0, attrs);
}
Sprite::Sprite(float posX, float posY, float width, float height, const Texture* texture) : GUIObject(posX, posY, width, height) {
	/* Mesh */
	int attrs[] = { 2, 2, 4, 0 };
	buffer = new float[GUI_VERTEX_SIZE * 6];
	mesh = new Mesh(buffer, 0, attrs);

	/* Texture */
	this->texture = new Texture(*texture);

	/* Shader */
	shader = load_shader("GUIVertex.glsl","GUIFragment.glsl");

	/* Buffer */
	int i = 0;
	GUI_RECT(i, posX, posY, width, height, 1.0f, 1.0f, 1.0f, 1.0f);

	/* Init */
	x = posX;
	y = posY;
}
void Sprite::render() {
	/* Texture */
	texture->bind();

	/* Shader */
	shader->use();

	/* Matrix init */
	float aspect = (float)Window::height / (float)Window::width;
	proj = glm::ortho(-1.0f, 1.0f, -aspect, aspect, 0.0f, 100.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

	/* Shader uniforms */
	shader->uniformMatrix("modelProj", glm::mat4(1.0f));

	/* Draw */
	mesh->reload(buffer, 6);
	mesh->draw(GL_TRIANGLES);
}
void Sprite::cleanUp() {
	texture->deleteTex();
	delete mesh;
	delete texture;
	delete[] buffer;
}