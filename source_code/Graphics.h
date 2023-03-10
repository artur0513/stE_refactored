﻿#pragma once
#include "Basics.h"
#include "ResourceManager.h"
#include "Console.h"
#include "json.hpp"

using json = nlohmann::json;

namespace gr {

	void json_to_coord(sf::Vector2d& pos, sf::Vector2d& size, json & _json);

	std::pair<sf::Sprite, sf::Time> json_to_frame(json& _json);

	enum class SPRITE_TYPE : bool {
		DIFFUSE,
		HEIGHT
	};

	// Класс похожий на sf::Rect<double>, но чуть более мне удобный
	class Rect {
	public:
		sf::Vector2d pos, size; // pos = левый верхний угол

		Rect();

		Rect(double pos_x, double pos_y, double size_x, double size_y);

		Rect(sf::Vector2d _pos, sf::Vector2d _size);

		bool intersects(const Rect* rect) const; // Проверка пересечения в другим прямоугольником 
	};


	class Camera : public Rect {
	public:
		double aspect_ratio;
		sf::Vector2f render_res;

		sf::Vector2f world_to_pixel_pos(sf::Vector2d world_pos) const;

		sf::Vector2f world_to_pixel_size(sf::Vector2d world_size) const;

		sf::Vector2d pixel_to_world_pos(sf::Vector2f camera_pos) const;

		sf::Vector2f world_to_glsl_pos(sf::Vector2d world_pos) const;
	};


	class Animation {
	private:
		std::deque<std::pair<sf::Sprite, sf::Time>> frames;
		sf::Time full_anim_time = sf::milliseconds(0);

	public:
		void add_frame(sf::Sprite spr, sf::Time t);

		void add_frame(std::pair<sf::Sprite, sf::Time> frame);

		sf::Sprite* get_frame(sf::Time time);
	};

	class Drawable : public Rect {
	protected:
		double layer = pos.y - size.y, layer_shift = 0.0;

	public:
		virtual bool var_update(const Camera& cam) = 0;

		virtual void animation_update() = 0;

		virtual void load_from_file(json& _json) = 0;

		virtual void draw(sf::RenderTarget* target) = 0;

		double get_layer();
	};

	class SpriteObject final : public Drawable {
	private:
		Console* con = Console::get_instance();

		sf::Sprite* current_sprite;
		std::deque<std::pair<Animation, std::string>> anims; // Все анимации спрайта. Теперь нету default_sprite, он как-бы просто лежит в anims[0]

		size_t current_anim = 0;
		float anim_speed_factor = 1.0f;

		sf::Clock clock;
		sf::Time anim_time;

		bool is_valid = false; // Корректно ли загружен обьект и загружен ли вообще, т.е. можно ли его использовать
		sf::Texture* texture;
	public:
		std::string name;
		SPRITE_TYPE type;

		bool set_anim(size_t anim_num); // Пробуем врубить анимации под заданным номером. true если анимация существует

		bool var_update(const Camera& cam) override final;

		void animation_update() override final;

		void draw(sf::RenderTarget* target) override final;

		void load_from_file(json& _json) override final;

		void print_info();
	};

	class Effect : public Drawable {

	};

	class Particle : public Drawable {

	};

	class LightSource : public Drawable {

	};

	// Разметка классов очень примерная пока
	class GraphicsEngine {
	private:
		const sf::Color clear_color = sf::Color(0, 0, 0, 0);
		const int MAX_LIGHT_COUNT = 32;
		const int SHADER_UPD_FREQ = 20;

		Console* con = Console::get_instance();

		sf::Sprite render_result;

		Camera cam;

		std::deque<Drawable*> objects; // Все обьекты на карте
		std::deque<Drawable*> render_queue; // Очередь отрисовки

		sf::Vector2u render_res, output_res;

		//sf::Glsl::Mat3 color_correction; // !!! Вынести все постэффекты в отдельный класс !!!!

		sf::RenderTexture rtexture1, rtexture2; // Теперь будет не много рендер-текстур, а 2, которые поочередно меняем друг с другом

		void(*glTextureBarrier); //краеугольный камень для карты высот

		sf::RenderTexture lightmap, effectbuffer;
		sf::Shader lightmapShader, combineShader, posteffectShader, heightShader;

		void createRenderQueue(); // Создать очередь отрисовки = найти все обьекты в кадре

		void sortRenderQueue(); // Отсортировать все обьекты в кадре = diffuse, height, effect по layer, свет в конец
		// Всего обьектов в кадре даже если 1000 будет, сортировка будет ультра быстрой, и можно все типы обьектов в кучу пихать

		void renderLightSource(LightSource* src); // Для каждого источника вызываем шейдер и рисуем одну строчку в lightmap

		void set_uniform(); // Отправить часть параметров (погодных, эффектов) в шейдер (ту часть, что можно обновлять не каждый кадр)

	public:
		GraphicsEngine();

		void render();

		void load_from_file(json& _json);

		sf::Sprite& get_sprite();

		~GraphicsEngine();
	};

}