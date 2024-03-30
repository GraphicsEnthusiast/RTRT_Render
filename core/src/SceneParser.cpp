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

	json lightData = data.value("light", json());
	ParseLight(lightData, scene);
}

void SceneParser::ParseCamera(const json& data) {
	cout << "Camera Information: " << endl;

	camera.medium = -1;

	json look_from_j = data.value("look_from", json());
	vec3f look_from(look_from_j.at(0), look_from_j.at(1), look_from_j.at(2));
	cout << "look_from: " << look_from.x << "   " << look_from.y << "   " << look_from.z << "   " << endl;
	camera.from = look_from;

	json up_j = data.value("up", json());
	vec3f up(up_j.at(0), up_j.at(1), up_j.at(2));
	cout << "up: " << up.x << "   " << up.y << "   " << up.z << "   " << endl;
	camera.up = up;
}

void SceneParser::ParseLight(const json& data, Scene& scene) {
	cout << "Lights Information: " << endl;
	int i = 0;
	for (auto iter = data.cbegin(); iter != data.cend(); ++iter) {
		json light = data.at(i);
		string type = light.value("type", "");
		cout << "type: " << type << endl;
		if (type == "infinite") {
			string hdr_path = light.value("hdr_path", "");

			cout << "hdr_path: " << hdr_path << endl;

			scene.AddEnv(hdr_path);
		}
		else if (type == "quad") {
// 			json pos_j = light.value("pos", json());
// 			vec3 pos(pos_j.at(0), pos_j.at(1), pos_j.at(2));
// 			cout << "pos: " << to_string(pos) << endl;
// 
// 			json u_j = light.value("u", json());
// 			vec3 u(u_j.at(0), u_j.at(1), u_j.at(2));
// 			cout << "u: " << to_string(u) << endl;
// 
// 			json v_j = light.value("v", json());
// 			vec3 v(v_j.at(0), v_j.at(1), v_j.at(2));
// 			cout << "v: " << to_string(v) << endl;
// 
// 			string material_name = light.value("material", "");
// 			cout << "material: " << material_name << endl;
// 
// 			auto mat = SearchMaterial(material_name);
// 
// 			string medium_int_name = light.value("medium_int", "");
// 			cout << "medium_int: " << medium_int_name << endl;
// 
// 			string medium_ext_name = light.value("medium_ext", "");
// 			cout << "medium_ext: " << medium_ext_name << endl;
// 
// 			auto me_int = SearchMedium(medium_int_name);
// 			auto me_ext = SearchMedium(medium_ext_name);
// 
// 			auto quad = new Quad(mat, pos, u, v, me_ext, me_int);
// 			auto quad_light = make_shared<QuadLight>(quad);
// 
// 			lights.push_back(quad_light);
		}
		else if (type == "sphere") {
// 			json center_j = light.value("center", json());
// 			vec3 center(center_j.at(0), center_j.at(1), center_j.at(2));
// 			cout << "center: " << to_string(center) << endl;
// 
// 			float radius = light.value("radius", 0.0f);
// 			cout << "radius: " << radius << endl;
// 
// 			string material_name = light.value("material", "");
// 			cout << "material: " << material_name << endl;
// 
// 			auto mat = SearchMaterial(material_name);
// 
// 			string medium_int_name = light.value("medium_int", "");
// 			cout << "medium_int: " << medium_int_name << endl;
// 
// 			string medium_ext_name = light.value("medium_ext", "");
// 			cout << "medium_ext: " << medium_ext_name << endl;
// 
// 			auto me_int = SearchMedium(medium_int_name);
// 			auto me_ext = SearchMedium(medium_ext_name);
// 
// 			auto sphere = new Sphere(mat, center, radius, me_ext, me_int);
// 			auto sphere_light = make_shared<SphereLight>(sphere);
// 
// 			lights.push_back(sphere_light);
		}

		cout << endl;
		i++;
	}
}
