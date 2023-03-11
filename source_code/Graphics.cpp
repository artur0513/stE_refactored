#include "Graphics.h"
#include <exception>
#include <typeinfo>

// json load functions
void gr::json_to_coord(sf::Vector2d& pos, sf::Vector2d& size, json& _json) {
	try {
		pos = sf::Vector2d(_json[0].get<double>(), _json[1].get<double>());
		size = sf::Vector2d(_json[2].get<double>(), _json[3].get<double>());
	} catch (nlohmann::detail::type_error err) {
		throw err;
	}
}

std::pair<sf::Sprite, sf::Time> gr::json_to_frame(json& _json) {
	sf::Sprite spr;
	sf::IntRect rect;
	float time = 0.f;
	try {
		rect.left = _json[0].get<double>();
		rect.top = _json[1].get<double>();

		rect.width = _json[2].get<double>();
		rect.height = _json[3].get<double>();

		time = _json[4].get<double>();
	}
	catch (nlohmann::detail::type_error err) {
		throw err;
	}
	spr.setTextureRect(rect);
	return std::pair<sf::Sprite, sf::Time>(spr, sf::milliseconds(time));
}

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

void gr::Animation::add_frame(std::pair<sf::Sprite, sf::Time> frame) {
	full_anim_time += frame.second;
	frame.second = full_anim_time;
	frames.push_back(frame);
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

// Drawable class

double gr::Drawable::get_layer() {
	return layer;
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
	return true;
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
	clock.restart();
	
	try {
		current_sprite = anims[current_anim].first.get_frame(anim_time);
	} catch (std::logic_error& e) {
		con->log(get_message_prefix(this) + " Animation error (currnet_anim =" + std::to_string(current_anim) + "):" + e.what(), ConsoleMessageType::ERR);
	}
	return;
}

void gr::SpriteObject::draw(RTextures& rtex) {
	if (type == SPRITE_TYPE::DIFFUSE)
		rtex.diffuse.draw(*current_sprite);
	else if (type == SPRITE_TYPE::HEIGHT)
		rtex.height.draw(*current_sprite);
}

void gr::SpriteObject::load_from_file(json& _json) {
	std::string texture_name, sprite_type;
	try {
		texture_name = _json["texture"].get<std::string>();
	} catch(nlohmann::detail::type_error err) {
		con->log(get_message_prefix(this) + " Json \"texture\" parameter type != string, or it does not exist", ConsoleMessageType::ERR);
		return;
	}

	try {
		sprite_type = _json["sprite_type"].get<std::string>();
		if (sprite_type == "diffuse")
			type = SPRITE_TYPE::DIFFUSE;
		else if (sprite_type == "height")
			type = SPRITE_TYPE::HEIGHT;
	}
	catch (nlohmann::detail::type_error err) {
		con->log(get_message_prefix(this) + " Json \"sprite_type\" parameter type != string, or it does not exist, set to diffuse as defalt", ConsoleMessageType::ERR);
		type = SPRITE_TYPE::DIFFUSE;
	}

	texture = Resource_manager<sf::Texture>::get_instance()->get_object(texture_name);
	if (texture == nullptr) {
		con->log(get_message_prefix(this) + " Error loading texture from file " + _json["texture"].get<std::string>());
		return;
	}

	try {
		layer_shift = _json["layer_shift"].get<double>();
	} catch (nlohmann::detail::type_error err) {
		con->log(get_message_prefix(this) + " Json \"layer_shift\" parameter type != double, or it does not exist. It is automaticly set to 0.0", ConsoleMessageType::ERR);
		layer_shift = 0.0;
	}

	try {
		json_to_coord(pos, size, _json["coordinates"]);
	} catch (nlohmann::detail::type_error err) {
		con->log(get_message_prefix(this) + " Json \"coordiantes\" parameter type error. Should be 4 float numbers", ConsoleMessageType::ERR);
		return;
	}

	for (auto& anim : _json["anims"].items()) {
		Animation new_anim;
		for (auto& frame : anim.value()) {
			try {
				auto spr = json_to_frame(frame);
				spr.first.setTexture(*texture);
				new_anim.add_frame(spr);
			} catch (nlohmann::detail::type_error err) {
				con->log(get_message_prefix(this) + " Json error while loading animations", ConsoleMessageType::ERR);
				return;
			}
		}
		anims.push_back(std::pair<Animation, std::string>(new_anim, anim.key()));
	}
	is_valid = true;
	return;
}

void gr::SpriteObject::print_info() {
	con->log(get_message_prefix(this) + " pos on screen: " + std::to_string(current_sprite->getPosition().x) + ", " + std::to_string(current_sprite->getPosition().y));
}

// Effect class

// Particle class

// LightSource class

// GraphicsEngine class

gr::GraphicsEngine::GraphicsEngine() {
	rtex.diffuse.create(sf::Vector2u(1920, 1080)); // ����� ��������
	rtex.height.create(sf::Vector2u(1920, 1080)); // ����� ��������
	rtex.effectbuffer.create(sf::Vector2u(1920, 1080)); // ����� ��������
	rtex.lightmap.create(sf::Vector2u(2048, 64)); // ����� ��������

	render_result.setTexture(rtex.diffuse.getTexture());

	cam.pos = sf::Vector2d(-8, 4.5);
	cam.size = sf::Vector2d(16, 9);
	cam.render_res = sf::Vector2f(1920, 1080);
}

void gr::GraphicsEngine::createRenderQueue() {
	render_queue.clear();
	for (auto obj : objects)
		if (obj->var_update(cam))
			render_queue.push_back(obj);
}

void gr::GraphicsEngine::sortRenderQueue() {
	sort(render_queue.begin(), render_queue.end(),
		[](Drawable* obj1, Drawable* obj2) {return (obj1->get_layer() > obj2->get_layer()); });
}

void gr::GraphicsEngine::render() {
	rtex.diffuse.clear(clear_color);

	createRenderQueue();
	sortRenderQueue();

	for (auto obj : render_queue) {
		obj->draw(rtex);
	}

	rtex.diffuse.display();
}

sf::Sprite& gr::GraphicsEngine::get_sprite() {
	return render_result;
}

void gr::GraphicsEngine::load_from_file(json& _json) {
	sf::Clock clock;
	con->log("Loading " + std::to_string(_json.size()) + " graphics objects...", ConsoleMessageType::INFO);

	for (auto& json_obj : _json.items()) {
		std::string type = json_obj.value()["type"].get<std::string>(); // maybe try catch ? 
		if (type == "LightSource") {
			
		}
		else if (type == "Particle") {

		}
		else if (type == "Effect") {

		}
		else if (type == "SpriteObject") {
			Drawable* obj = new SpriteObject;
			obj->load_from_file(json_obj.value());
			obj->name = json_obj.key();
			objects.push_back(obj);
		}
	}

	con->log("Graphics objects loading done! Time in milliseconds (except parsing): " + std::to_string(clock.getElapsedTime().asMilliseconds()), ConsoleMessageType::SUCC);
	Resource_manager<sf::Texture>::get_instance()->print();
}

gr::GraphicsEngine::~GraphicsEngine() {
	for (auto obj : objects)
		delete obj;
}