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

	json meshData = data.value("mesh", json());
	ParseMesh(meshData, scene);
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
	cout << "Light Information: " << endl;
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
			json pos_j = light.value("pos", json());
			vec3f pos(pos_j.at(0), pos_j.at(1), pos_j.at(2));
			cout << "pos: " << pos.x << "   " << pos.y << "   " << pos.z << "   " << endl;

			json u_j = light.value("u", json());
			vec3f u(u_j.at(0), u_j.at(1), u_j.at(2));
			cout << "u: " << u.x << "   " << u.y << "   " << u.z << "   " << endl;

			json v_j = light.value("v", json());
			vec3f v(v_j.at(0), v_j.at(1), v_j.at(2));
			cout << "v: " << v.x << "   " << v.y << "   " << v.z << "   " << endl;

			json radiance_j = light.value("pos", json());
			vec3f radiance(radiance_j.at(0), radiance_j.at(1), radiance_j.at(2));
			cout << "radiance: " << radiance.x << "   " << radiance.y << "   " << radiance.z << "   " << endl;

			Light light;
			light.medium = -1;
			light.type = LightType::Quad;
			light.position = pos;
			light.u = u;
			light.v = v;
			light.radiance = radiance;

			scene.AddLight(light);
		}
		else if (type == "sphere") {
			json center_j = light.value("center", json());
			vec3f center(center_j.at(0), center_j.at(1), center_j.at(2));
			cout << "center: " << center.x << "   " << center.y << "   " << center.z << "   " << endl;

			float radius = light.value("radius", 0.0f);
			cout << "radius: " << radius << endl;

			json radiance_j = light.value("pos", json());
			vec3f radiance(radiance_j.at(0), radiance_j.at(1), radiance_j.at(2));
			cout << "radiance: " << radiance.x << "   " << radiance.y << "   " << radiance.z << "   " << endl;

			Light light;
			light.medium = -1;
			light.type = LightType::Sphere;
			light.position = center;
			light.radius = radius;
			light.radiance = radiance;

			scene.AddLight(light);
		}

		cout << endl;
		i++;
	}
}

void SceneParser::ParseMesh(const json& data, Scene& scene) {
	cout << "Mesh Information: " << endl;
	int i = 0;
	for (auto iter = data.cbegin(); iter != data.cend(); ++iter) {
		json mesh = data.at(i);

		string mesh_path = mesh.value("mesh_path", "");
		cout << "mesh_path: " << mesh_path << endl;

		string material_name = mesh.value("material", "");
		cout << "material: " << material_name << endl;

		string medium_int_name = mesh.value("medium_int", "");
		cout << "medium_int: " << medium_int_name << endl;

		string medium_ext_name = mesh.value("medium_ext", "");
		cout << "medium_ext: " << medium_ext_name << endl;

// 		auto me_int = SearchMedium(medium_int_name);
// 		auto me_ext = SearchMedium(medium_ext_name);
// 		auto mat = SearchMaterial(material_name);
// 
// 		auto mesh = new TriangleMesh(mat, mesh_path, model_tran, me_ext, me_int);
// 
// 		shapes.push_back(mesh);

		cout << endl;
		i++;
	}
}

void SceneParser::ParseMaterial(const json& data, Scene& scene) {

}
