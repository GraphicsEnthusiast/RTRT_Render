#include "SceneParser.h"

void SceneParser::LoadFromJson(const string& fn, Scene& scene, bool& is_succeed) {
	json data = CreateJsonFromFile(fn, is_succeed);
	if (!is_succeed) {
		return;
	}
	Parse(data, scene);
}

void SceneParser::Parse(const json& data, Scene& scene) {
	width = data.value("width", 1024);
	height = data.value("height", 1024);
	depth = data.value("depth", 4);

	cout << "Settings Information: " << endl;
	cout << "width: " << width << endl;
	cout << "height: " << height << endl;
	cout << "depth: " << depth << endl;

	json cameraData = data.value("camera", json());
	ParseCamera(cameraData);
}

void SceneParser::ParseCamera(const json& data) {
	cout << "Camera Information: " << endl;

// 	string medium_name = data.value("medium", "");
// 	cout << "medium: " << medium_name << endl;
// 	auto me = SearchMedium(medium_name);

	json look_from_j = data.value("look_from", json());
	vec3f look_from(look_from_j.at(0), look_from_j.at(1), look_from_j.at(2));
	cout << "look_from: " << look_from.x << "   " << look_from.y << "   " << look_from.z << "   " << endl;
	camera.from = look_from;

	json up_j = data.value("up", json());
	vec3f up(up_j.at(0), up_j.at(1), up_j.at(2));
	cout << "up: " << up.x << "   " << up.y << "   " << up.z << "   " << endl;
	camera.up = up;
}
