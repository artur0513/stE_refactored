#include "Graphics.h"

// Rect class

gr::Rect::Rect() {
	pos = sf::Vector2d(0.0, 0.0);
	size = sf::Vector2d(0.0, 0.0);
};

gr::Rect::Rect(double pos_x, double pos_y, double size_x, double size_y) {
	pos = sf::Vector2d(pos_x, pos_y);
	size = sf::Vector2d(size_x, size_y);
};

gr::Rect::Rect(sf::Vector2d _pos, sf::Vector2d _size) {
	pos = _pos;
	size = _size;
};

bool gr::Rect::intersects(const gr::Rect& rect) {
	Rect temp = rect;
	temp.size += size;
	temp.pos.x -= size.x;
	temp.pos.y += size.y;

	return (pos.x < temp.size.x + temp.pos.x) && (pos.x > temp.pos.x) && (pos.y > temp.pos.y - temp.size.y) && (pos.y < temp.pos.y);
}

// Camera class

sf::Vector2f gr::Camera::world_to_pixel_pos(sf::Vector2d world_pos) {
	return sf::Vector2f((world_pos.x - pos.x) * render_res.x / size.x, (pos.y - world_pos.y) * render_res.y / size.y);
}

sf::Vector2d gr::Camera::pixel_to_world_pos(sf::Vector2f camera_pos) {
	return sf::Vector2d(pos.x + size.x * double(camera_pos.x / render_res.x), pos.y - size.y * double(camera_pos.y / render_res.y));
}

sf::Vector2f gr::Camera::world_to_glsl_pos(sf::Vector2d world_pos) {
	return sf::Vector2f((world_pos.x - pos.x)/ size.x, (world_pos.y - pos.y + size.y) / size.y);
}

// Animation class