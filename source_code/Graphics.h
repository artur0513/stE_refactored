#pragma once
#include <SFML/Graphics.hpp>
#include "Basics.h"
#include <fstream>
#include <deque>
#include "ResourceManager.h"

namespace gr {

	// ����� ������� �� sf::Rect<double>, �� ���� ����� ��� �������
	class Rect {
	public:
		sf::Vector2d pos, size;

		Rect();

		Rect(double pos_x, double pos_y, double size_x, double size_y);

		Rect(sf::Vector2d _pos, sf::Vector2d _size);

		bool intersects(const Rect& rect); // �������� ����������� � ������ ��������������� 
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

		std::deque<Drawable*> objects; // ��� ������� �� �����
		std::deque<Drawable*> render_queue; // ������� ���������

		sf::Vector2u render_res, output_res;

		sf::Glsl::Mat3 color_correction; // !!! ������� ��� ����������� � ��������� ����� !!!!

		sf::RenderTexture rtexture1, rtexture2; // ������ ����� �� ����� ������-�������, � 2, ������� ���������� ������ ���� � ������

		void(*glTextureBarrier);

		sf::RenderTexture lightmap, effectbuffer;
		sf::Shader lightmapShader, combineShader, posteffectShader, heightShader;

		void createRenderQueue(); // ������� ������� ��������� = ����� ��� ������� � �����

		void sortRenderQueue(); // ������������� ��� ������� � ����� = diffuse, height, effect �� layer, ���� � �����
		// ����� �������� � ����� ���� ���� 1000 �����, ���������� ����� ������ �������, � ����� ��� ���� �������� � ���� ������

		void renderLightSource(LightSource* src); // ��� ������� ��������� �������� ������ � ������ ���� ������� � lightmap

		void set_uniform(); // ��������� ����� ���������� (��������, ��������) � ������ (�� �����, ��� ����� ��������� �� ������ ����)

	public:
		GraphicsEngine();

		void render();

		void load_from_file(std::ifstream& file);

		void get_sprite();

		~GraphicsEngine();
	};

}