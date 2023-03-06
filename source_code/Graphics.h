#pragma once
#include <SFML/Graphics.hpp>
#include "Basics.h"
#include <fstream>
#include <deque>
#include "ResourceManager.h"

namespace gr {

	// Класс похожий на sf::Rect<double>, но чуть более мне удобный
	class Rect {
	public:
		sf::Vector2d pos, size;

		Rect();

		Rect(double pos_x, double pos_y, double size_x, double size_y);

		Rect(sf::Vector2d _pos, sf::Vector2d _size);

		bool intersects(const Rect& rect); // Проверка пересечения в другим прямоугольником 
	};

	class Camera : public Rect {
	public:
		double aspect_ratio;
		sf::Vector2d center;

		sf::Vector2f world_to_camera_pos(sf::Vector2d gl_pos);

		sf::Vector2d camera_to_world_pos(sf::Vector2f c_pos);

		sf::Vector2f world_to_glsl_pos(sf::Vector2d gl_pos);
	};

	class Animation {

	};

	class Drawable : public Rect {
	public:
		virtual void var_update(const Camera& cam);

		virtual void animation_update();

		virtual void load_from_file(std::ifstream& file);

		virtual void draw(sf::RenderTarget* target);
	};

	class SpriteObject : public Drawable {

	};

	class Effect : public Drawable {

	};

	class LightSource : public Drawable {

	};

	class GraphicsEngine {
	private:
		const sf::Color clear_color = sf::Color(0, 0, 0, 0);
		const int MAX_LIGHT_COUNT = 32;
		const int SHADER_UPD_FREQ = 20;

		Camera cam;

		std::deque<Drawable*> objects; // Все обьекты на карте
		std::deque<Drawable*> render_queue; // Очередь отрисовки

		sf::Vector2u render_res, output_res;

		sf::Glsl::Mat3 color_correction; // !!! Вынести все постэффекты в отдельный класс !!!!

		sf::RenderTexture rtexture1, rtexture2; // Теперь будет не много рендер-текстур, а 2, которые поочередно меняем друг с другом

		void(*glTextureBarrier);

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

		void load_from_file(std::ifstream& file);

		void get_sprite();

		~GraphicsEngine();
	};

}