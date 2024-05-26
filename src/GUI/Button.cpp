#include "Button.h"
#include "GUI.h"
#include "../EventHandler.h"
#include "../Window.h"
#include "../defines.cpp"
#include "Text.h"

Button::Button() {
	int attrs[] = { 2, 2, 4, 0 };
	state = Default;
	buffer = new float[GUI_VERTEX_SIZE * 6];
	mesh = new Mesh(buffer, 0, attrs);
}
Button::Button(float posX, float posY, float width, float height,
			   const Texture* defTexture,
			   const Texture* hoverTexture,
			   const Texture* clickedTexture,
			   std::string text,
			   std::function<void()> function)
	: GUIObject(posX, posY, width, height)
	, function(function) {

	/* Mesh */
	int attrs[] = { 2, 2, 4, 0 };
	buffer = new float[GUI_VERTEX_SIZE * 6];
	mesh = new Mesh(buffer, 0, attrs);

	/* Textures */
	textures[Default] = new Texture(*defTexture);
	textures[onHover] = new Texture(*hoverTexture);
	textures[onClick] = new Texture(*clickedTexture);
	state = Default;

	/* Sahder */
	shader = load_shader("GUIVertex.glsl", "GUIFragment.glsl");

	/* Buffer */
	int i = 0;
	GUI_RECT(i, posX, posY, width, height, 1.0f, 1.0f, 1.0f, 1.0f);

	/* Coords */
	x = posX;
	y = posY;
	w = width;
	h = height;

	/* Texts */
	this->text = new Text(text, glm::vec2(posX, posY), 1.0f);
}
void Button::render() {
	/* Bind texture */
	textures[state]->bind();

	/* Use shader program */
	shader->use();

	/* Matrix init */
	float aspect = (float)Window::height / (float)Window::width;
	proj = glm::ortho(-1.0f, 1.0f, -aspect, aspect, 0.0f, 100.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

	/* Uniforms */
	shader->uniformMatrix("modelProj", glm::mat4(1.0f));

	/* Draw */
	mesh->reload(buffer, 6);
	mesh->draw(GL_TRIANGLES);
	text->render();
}
void Button::refreshState() {
	float mouseX, mouseY;

	/* Window coords to OpenGL coords */
	mouseX = (Events::x / Window::width - 0.5f) * 2.0f;
	mouseY = -(Events::y / Window::height - 0.5f) * 2.0f;

	/* If mouse inside AABB */
	if (mouseX >= x - w / 2 && mouseX <= x + w / 2 &&
		mouseY >= y - h / 2 && mouseY <= y + h / 2) {
		if (Events::isClicked(GLFW_MOUSE_BUTTON_1)) {
			state = onClick;
		} else {
			if (state == onClick) function();

			state = onHover;
		}
	}
	/* If mouse outside AABB */
	else {
		if (state == onClick) function();

		state = Default;
	}
}
void Button::cleanUp() {
	for (int i = 0; i < 3; i++) {
		textures[i]->deleteTex();
		delete textures[i];
	}
	delete mesh;
	delete[] buffer;
}