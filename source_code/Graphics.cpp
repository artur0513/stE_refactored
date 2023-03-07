#include "Graphics.h"
#include <exception>

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

bool gr::Rect::intersects(const gr::Rect* rect) const {
	Rect temp(rect->pos, rect->size);
	temp.size += size;
	temp.pos.x -= size.x;
	temp.pos.y += size.y;

	return (pos.x < temp.size.x + temp.pos.x) && (pos.x > temp.pos.x) && (pos.y > temp.pos.y - temp.size.y) && (pos.y < temp.pos.y);
}

// Camera class

sf::Vector2f gr::Camera::world_to_pixel_pos(sf::Vector2d world_pos) const {
	return sf::Vector2f((world_pos.x - pos.x) * render_res.x / size.x, (pos.y - world_pos.y) * render_res.y / size.y);
}

sf::Vector2f gr::Camera::world_to_pixel_size(sf::Vector2d world_size) const {
	return sf::Vector2f(world_size.x * render_res.x / size.x, world_size.y * render_res.y / size.y);
}

sf::Vector2d gr::Camera::pixel_to_world_pos(sf::Vector2f camera_pos) const {
	return sf::Vector2d(pos.x + size.x * double(camera_pos.x / render_res.x), pos.y - size.y * double(camera_pos.y / render_res.y));
}

sf::Vector2f gr::Camera::world_to_glsl_pos(sf::Vector2d world_pos) const {
	return sf::Vector2f((world_pos.x - pos.x)/ size.x, (world_pos.y - pos.y + size.y) / size.y);
}

// Animation class

void gr::Animation::add_frame(sf::Sprite spr, sf::Time t) {
	full_anim_time += t;
	frames.push_back(std::pair<sf::Sprite, sf::Time>(spr, full_anim_time));
}

sf::Sprite* gr::Animation::get_frame(sf::Time time) {
	if (frames.size() == 0)
		throw std::logic_error("Animation size was 0 frames");
	if (full_anim_time == sf::milliseconds(0))
		throw std::logic_error("Full animation time was 0");

	time %= full_anim_time;
	for (std::pair<sf::Sprite, sf::Time>& frame : frames)
		if (time < frame.second)
			return &frame.first;

	return &frames[0].first;
}

// SpriteObject class

bool gr::SpriteObject::var_update(const gr::Camera& cam) {
	if (!cam.intersects(this))
		return false;

	animation_update();

	current_sprite->setPosition(cam.world_to_pixel_pos(pos));
	sf::Vector2f new_pixel_size = cam.world_to_pixel_size(size);
	current_sprite->setScale(sf::Vector2f(new_pixel_size.x / current_sprite->getLocalBounds().width, new_pixel_size.y / current_sprite->getLocalBounds().height));

	layer = pos.y - size.y + layer_shift;
}

bool gr::SpriteObject::set_anim(size_t anim_num) {
	if (anim_num < anims.size()) {
		current_anim = anim_num;
		clock.restart();
		anim_time = sf::milliseconds(0);
		return true;
	}
	return false;
}

void gr::SpriteObject::animation_update() {
	anim_time += clock.getElapsedTime() * anim_speed_factor;
	
	try {
		current_sprite = anims[current_anim].get_frame(anim_time);
	} catch (std::logic_error& e) {
		con->log(get_message_prefix(this) + " Animation error (currnet_anim =" + std::to_string(current_anim) + "):" + e.what());
	}
	return;
}

void gr::SpriteObject::draw(sf::RenderTarget* target) {
	target->draw(*current_sprite);
}