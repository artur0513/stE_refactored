#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <filesystem>

// �����-�������� ��� �������� ������� � ��������, ��������� �������������� ������� �������� ���������� �������/�������� � ������ ��� ��������
template<class T>
class Resource_manager {
private:
	std::map<std::string, T*> resources;

	Resource_manager() {};
	Resource_manager(const Resource_manager& r) {};
	Resource_manager operator=(const Resource_manager& r) {};
	Resource_manager(Resource_manager&& r) {};
	Resource_manager& operator=(const Resource_manager&& r) {};

	bool load_object_from_file(T* obj, std::string filename) {}; // ����� �������������� �������������� ��� ������� �������� ��� ��� T
public:
	static Resource_manager* get_instance() {
		static Resource_manager instance;
		return &instance;
	}

	void preload(std::string filename) {
		std::ifstream file(filename.c_str());
		if (file.is_open()) {
			std::string object_filename;
			while (getline(file, object_filename)) { // ��������� �������� ����� �������
				if (object_filename.size() == 0) // ���� ������ ������ 0
					continue;
				else if (object_filename[object_filename.find_first_not_of(" ")] == '#') // ��� ��� ���������� � ������� ��� �����������
					continue; // �� ��������� � ��������� ������

				object_filename = std::filesystem::relative(object_filename).string(); // ������� �������� ����� � ������� ����
				T* new_object = new T; // ������� ��������� �� ������

				if (!load_object_from_file(new_object, object_filename)) // ������� ������� �� �����
					delete new_object; // ���� �� �������, �� ������� � ���� ������
				else {
					resources.insert(std::pair<std::string, T*>(object_filename, new_object)); // ����� ���������� � ����� � ���� ���� ������
				}
			}
		}
		else {
			std::cout << "cannot load objects, file '" << filename << "' not found!" << std::endl;
		}
		file.close();
	}

	T* get_object(std::string filename) {
		filename = std::filesystem::relative(filename).string(); // ������� �������� ����� � ������� ����
		auto itrtr = resources.find(filename);

		if (itrtr == resources.end()) { // ���� ������� ������� ����, �� ��������� ��� �����, �� ������ ����� ����� ������� �������
			T* new_object = new T; // ������� ��������� �� ������

			if (!load_object_from_file(new_object, filename)) { // ������� ������� �� �����
				return nullptr;
			}
			else {
				resources.insert(std::pair<std::string, T*>(filename, new_object)); // ����� ���������� � �����
				return new_object; // � ���������� ��������� �� ����� ������
			}
		}
		else // ���� ������ ��� �� �����, �� ���������� �� ���� ���������
			return itrtr->second;
	}

	~Resource_manager() {
		auto itrtr = resources.begin();
		while (itrtr != resources.end()) {
			delete itrtr->second;
			itrtr++;
		}
	}
};