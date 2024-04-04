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

	cout << "Settings Information: " << endl;
	cout << "width: " << width << endl;
	cout << "height: " << height << endl;

	json lightData = data.value("light", json());
	ParseLight(lightData, scene);

	json materialData = data.value("material", json());
	ParseMaterial(materialData, scene);

	json mediumData = data.value("medium", json());
	ParseMedium(mediumData, scene);

	json meshData = data.value("mesh", json());
	ParseMesh(meshData, scene);

	json cameraData = data.value("camera", json());
	ParseCamera(cameraData, scene);

	worldScale = length(scene.bounds.span());

	cout << endl;
}

void SceneParser::ParseCamera(const json& data, Scene& scene) {
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

	camera.medium = -1;
	camera.at = scene.bounds.center();
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

		int mi_int = medium_int_name == "" ? -1 : mediumTable[medium_int_name];
		int mi_ext = medium_ext_name == "" ? -1 : mediumTable[medium_ext_name];

		scene.AddMesh(
			mesh_path,
			materialTable[material_name].second,
			materialTable[material_name].first,
			mi_ext, mi_int
		);

		cout << endl;
		i++;
	}
}

void SceneParser::ParseMaterial(const json& data, Scene& scene) {
	cout << "Material Information: " << endl;
	int i = 0;
	for (auto iter = data.cbegin(); iter != data.cend(); ++iter) {
		json material = data.at(i);
		string type = material.value("type", "");
		cout << "type: " << type << endl;

		if (type == "diffuse") {
			string name = material.value("name", "");
			cout << "name: " << name << endl;

			string albedo_texture_type = material.value("albedo_texture_type", "");
			cout << "albedo_texture_type: " << albedo_texture_type << endl;

			TextureFile texf;
			Material m;
			m.type = MaterialType::Diffuse;

			if (albedo_texture_type == "constant") {
				json albedo_j = material.value("albedo", json());
				vec3f albedo(albedo_j.at(0), albedo_j.at(1), albedo_j.at(2));
				cout << "albedo: " << albedo.x << "   " << albedo.y << "   " << albedo.z << "   " << endl;

				m.albedo = albedo;
			}
			else if (albedo_texture_type == "image") {
				string albedo_path = material.value("albedo_path", "");
				cout << "albedo_path: " << albedo_path << endl;

				texf.albedoFile = albedo_path;
			}

			string roughness_texture_type = material.value("roughness_texture_type", "");
			cout << "roughness_texture_type: " << roughness_texture_type << endl;

			if (roughness_texture_type == "constant") {
				float roughness = material.value("roughness", 0.1f);
				cout << "roughness: " << roughness << endl;

				m.roughness = roughness;
			}
			else if (roughness_texture_type == "image") {
				string roughness_path = material.value("roughness_path", "");
				cout << "roughness_path: " << roughness_path << endl;

				texf.roughnessFile = roughness_path;
			}

			materialTable.insert({ name, {texf, m} });
		}
		else if (type == "conductor") {
			string name = material.value("name", "");
			cout << "name: " << name << endl;

			string albedo_texture_type = material.value("albedo_texture_type", "");
			cout << "albedo_texture_type: " << albedo_texture_type << endl;

			TextureFile texf;
			Material m;
			m.type = MaterialType::Conductor;

			if (albedo_texture_type == "constant") {
				json albedo_j = material.value("albedo", json());
				vec3f albedo(albedo_j.at(0), albedo_j.at(1), albedo_j.at(2));
				cout << "albedo: " << albedo.x << "   " << albedo.y << "   " << albedo.z << "   " << endl;

				m.albedo = albedo;
			}
			else if (albedo_texture_type == "image") {
				string albedo_path = material.value("albedo_path", "");
				cout << "albedo_path: " << albedo_path << endl;

				texf.albedoFile = albedo_path;
			}

			string roughness_texture_type = material.value("roughness_texture_type", "");
			cout << "roughness_texture_type: " << roughness_texture_type << endl;

			if (roughness_texture_type == "constant") {
				float roughness = material.value("roughness", 0.1f);
				cout << "roughness: " << roughness << endl;

				m.roughness = roughness;
			}
			else if (roughness_texture_type == "image") {
				string roughness_path = material.value("roughness_path", "");
				cout << "roughness_path: " << roughness_path << endl;

				texf.roughnessFile = roughness_path;
			}

			string anisotropy_texture_type = material.value("anisotropy_texture_type", "");
			cout << "anisotropy_texture_type: " << anisotropy_texture_type << endl;

			if (anisotropy_texture_type == "constant") {
				float anisotropy = material.value("anisotropy", 0.0f);
				cout << "anisotropy: " << anisotropy << endl;

				m.anisotropy = anisotropy;
			}
			else if (anisotropy_texture_type == "image") {
				string anisotropy_path = material.value("anisotropy_path", "");
				cout << "anisotropy_path: " << anisotropy_path << endl;

				texf.anisotropyFile = anisotropy_path;
			}

			json eta_j = material.value("eta", json());
			vec3f eta(eta_j.at(0), eta_j.at(1), eta_j.at(2));
			cout << "eta: " << eta.x << "   " << eta.y << "   " << eta.z << "   " << endl;
			m.eta = eta;

			json k_j = material.value("k", json());
			vec3f k(k_j.at(0), k_j.at(1), k_j.at(2));
			cout << "k: " << k.x << "   " << k.y << "   " << k.z << "   " << endl;
			m.k = k;

			materialTable.insert({ name, {texf, m} });
		}
		else if (type == "dielectric") {
			string name = material.value("name", "");
			cout << "name: " << name << endl;

			string albedo_texture_type = material.value("albedo_texture_type", "");
			cout << "albedo_texture_type: " << albedo_texture_type << endl;

			TextureFile texf;
			Material m;
			m.type = MaterialType::Dielectric;

			if (albedo_texture_type == "constant") {
				json albedo_j = material.value("albedo", json());
				vec3f albedo(albedo_j.at(0), albedo_j.at(1), albedo_j.at(2));
				cout << "albedo: " << albedo.x << "   " << albedo.y << "   " << albedo.z << "   " << endl;

				m.albedo = albedo;
			}
			else if (albedo_texture_type == "image") {
				string albedo_path = material.value("albedo_path", "");
				cout << "albedo_path: " << albedo_path << endl;

				texf.albedoFile = albedo_path;
			}

			string roughness_texture_type = material.value("roughness_texture_type", "");
			cout << "roughness_texture_type: " << roughness_texture_type << endl;

			if (roughness_texture_type == "constant") {
				float roughness = material.value("roughness", 0.1f);
				cout << "roughness: " << roughness << endl;

				m.roughness = roughness;
			}
			else if (roughness_texture_type == "image") {
				string roughness_path = material.value("roughness_path", "");
				cout << "roughness_path: " << roughness_path << endl;

				texf.roughnessFile = roughness_path;
			}

			string anisotropy_texture_type = material.value("anisotropy_texture_type", "");
			cout << "anisotropy_texture_type: " << anisotropy_texture_type << endl;

			if (anisotropy_texture_type == "constant") {
				float anisotropy = material.value("anisotropy", 0.0f);
				cout << "anisotropy: " << anisotropy << endl;

				m.anisotropy = anisotropy;
			}
			else if (anisotropy_texture_type == "image") {
				string anisotropy_path = material.value("anisotropy_path", "");
				cout << "anisotropy_path: " << anisotropy_path << endl;

				texf.anisotropyFile = anisotropy_path;
			}

			float int_ior = material.value("int_ior", 1.5f);
			cout << "int_ior: " << int_ior << endl;
			m.int_ior = int_ior;

			float ext_ior = material.value("ext_ior", 1.0f);
			cout << "ext_ior: " << ext_ior << endl;
			m.ext_ior = ext_ior;

			materialTable.insert({ name, {texf, m} });
		}
		else if (type == "thin_dielectric") {
			string name = material.value("name", "");
			cout << "name: " << name << endl;

			string albedo_texture_type = material.value("albedo_texture_type", "");
			cout << "albedo_texture_type: " << albedo_texture_type << endl;

			TextureFile texf;
			Material m;
			m.type = MaterialType::ThinDielectric;

			if (albedo_texture_type == "constant") {
				json albedo_j = material.value("albedo", json());
				vec3f albedo(albedo_j.at(0), albedo_j.at(1), albedo_j.at(2));
				cout << "albedo: " << albedo.x << "   " << albedo.y << "   " << albedo.z << "   " << endl;

				m.albedo = albedo;
			}
			else if (albedo_texture_type == "image") {
				string albedo_path = material.value("albedo_path", "");
				cout << "albedo_path: " << albedo_path << endl;

				texf.albedoFile = albedo_path;
			}

			string roughness_texture_type = material.value("roughness_texture_type", "");
			cout << "roughness_texture_type: " << roughness_texture_type << endl;

			if (roughness_texture_type == "constant") {
				float roughness = material.value("roughness", 0.1f);
				cout << "roughness: " << roughness << endl;

				m.roughness = roughness;
			}
			else if (roughness_texture_type == "image") {
				string roughness_path = material.value("roughness_path", "");
				cout << "roughness_path: " << roughness_path << endl;

				texf.roughnessFile = roughness_path;
			}

			string anisotropy_texture_type = material.value("anisotropy_texture_type", "");
			cout << "anisotropy_texture_type: " << anisotropy_texture_type << endl;

			if (anisotropy_texture_type == "constant") {
				float anisotropy = material.value("anisotropy", 0.0f);
				cout << "anisotropy: " << anisotropy << endl;

				m.anisotropy = anisotropy;
			}
			else if (anisotropy_texture_type == "image") {
				string anisotropy_path = material.value("anisotropy_path", "");
				cout << "anisotropy_path: " << anisotropy_path << endl;

				texf.anisotropyFile = anisotropy_path;
			}

			float int_ior = material.value("int_ior", 1.5f);
			cout << "int_ior: " << int_ior << endl;
			m.int_ior = int_ior;

			float ext_ior = material.value("ext_ior", 1.0f);
			cout << "ext_ior: " << ext_ior << endl;
			m.ext_ior = ext_ior;

			materialTable.insert({ name, {texf, m} });
		}
		else if (type == "plastic") {
			string name = material.value("name", "");
			cout << "name: " << name << endl;

			string albedo_texture_type = material.value("albedo_texture_type", "");
			cout << "albedo_texture_type: " << albedo_texture_type << endl;

			TextureFile texf;
			Material m;
			m.type = MaterialType::Plastic;

			if (albedo_texture_type == "constant") {
				json albedo_j = material.value("albedo", json());
				vec3f albedo(albedo_j.at(0), albedo_j.at(1), albedo_j.at(2));
				cout << "albedo: " << albedo.x << "   " << albedo.y << "   " << albedo.z << "   " << endl;

				m.albedo = albedo;
			}
			else if (albedo_texture_type == "image") {
				string albedo_path = material.value("albedo_path", "");
				cout << "albedo_path: " << albedo_path << endl;

				texf.albedoFile = albedo_path;
			}

			string roughness_texture_type = material.value("roughness_texture_type", "");
			cout << "roughness_texture_type: " << roughness_texture_type << endl;

			if (roughness_texture_type == "constant") {
				float roughness = material.value("roughness", 0.1f);
				cout << "roughness: " << roughness << endl;

				m.roughness = roughness;
			}
			else if (roughness_texture_type == "image") {
				string roughness_path = material.value("roughness_path", "");
				cout << "roughness_path: " << roughness_path << endl;

				texf.roughnessFile = roughness_path;
			}

			string anisotropy_texture_type = material.value("anisotropy_texture_type", "");
			cout << "anisotropy_texture_type: " << anisotropy_texture_type << endl;

			if (anisotropy_texture_type == "constant") {
				float anisotropy = material.value("anisotropy", 0.0f);
				cout << "anisotropy: " << anisotropy << endl;

				m.anisotropy = anisotropy;
			}
			else if (anisotropy_texture_type == "image") {
				string anisotropy_path = material.value("anisotropy_path", "");
				cout << "anisotropy_path: " << anisotropy_path << endl;

				texf.anisotropyFile = anisotropy_path;
			}

			float int_ior = material.value("int_ior", 1.5f);
			cout << "int_ior: " << int_ior << endl;
			m.int_ior = int_ior;

			float ext_ior = material.value("ext_ior", 1.0f);
			cout << "ext_ior: " << ext_ior << endl;
			m.ext_ior = ext_ior;

			string specular_texture_type = material.value("specular_texture_type", "");
			cout << "specular_texture_type: " << specular_texture_type << endl;

			if (specular_texture_type == "constant") {
				json specular_j = material.value("specular", json());
				vec3f specular(specular_j.at(0), specular_j.at(1), specular_j.at(2));
				cout << "specular: " << specular.x << "   " << specular.y << "   " << specular.z << "   " << endl;

				m.specular = specular;
			}
			else if (specular_texture_type == "image") {
				string specular_path = material.value("specular_path", "");
				cout << "specular_path: " << specular_path << endl;

				texf.specularFile = specular_path;
			}

			float nonlinear = material.value("nonlinear", true);
			cout << "nonlinear: " << nonlinear << endl;
			m.nonlinear = nonlinear;

			materialTable.insert({ name, {texf, m} });
		}
		else if (type == "clearcoated_conductor") {
			string name = material.value("name", "");
			cout << "name: " << name << endl;

			string albedo_texture_type = material.value("albedo_texture_type", "");
			cout << "albedo_texture_type: " << albedo_texture_type << endl;

			TextureFile texf;
			Material m;
			m.type = MaterialType::ClearCoatedConductor;

			if (albedo_texture_type == "constant") {
				json albedo_j = material.value("albedo", json());
				vec3f albedo(albedo_j.at(0), albedo_j.at(1), albedo_j.at(2));
				cout << "albedo: " << albedo.x << "   " << albedo.y << "   " << albedo.z << "   " << endl;

				m.albedo = albedo;
			}
			else if (albedo_texture_type == "image") {
				string albedo_path = material.value("albedo_path", "");
				cout << "albedo_path: " << albedo_path << endl;

				texf.albedoFile = albedo_path;
			}

			string roughness_texture_type = material.value("roughness_texture_type", "");
			cout << "roughness_texture_type: " << roughness_texture_type << endl;

			if (roughness_texture_type == "constant") {
				float roughness = material.value("roughness", 0.1f);
				cout << "roughness: " << roughness << endl;

				m.roughness = roughness;
			}
			else if (roughness_texture_type == "image") {
				string roughness_path = material.value("roughness_path", "");
				cout << "roughness_path: " << roughness_path << endl;

				texf.roughnessFile = roughness_path;
			}

			string anisotropy_texture_type = material.value("anisotropy_texture_type", "");
			cout << "anisotropy_texture_type: " << anisotropy_texture_type << endl;

			if (anisotropy_texture_type == "constant") {
				float anisotropy = material.value("anisotropy", 0.0f);
				cout << "anisotropy: " << anisotropy << endl;

				m.anisotropy = anisotropy;
			}
			else if (anisotropy_texture_type == "image") {
				string anisotropy_path = material.value("anisotropy_path", "");
				cout << "anisotropy_path: " << anisotropy_path << endl;

				texf.anisotropyFile = anisotropy_path;
			}

			json eta_j = material.value("eta", json());
			vec3f eta(eta_j.at(0), eta_j.at(1), eta_j.at(2));
			cout << "eta: " << eta.x << "   " << eta.y << "   " << eta.z << "   " << endl;
			m.eta = eta;

			json k_j = material.value("k", json());
			vec3f k(k_j.at(0), k_j.at(1), k_j.at(2));
			cout << "k: " << k.x << "   " << k.y << "   " << k.z << "   " << endl;
			m.k = k;

			string roughness_u_texture_type = material.value("roughness_u_texture_type", "");
			shared_ptr<Texture> roughness_u_texture;
			cout << "roughness_u_texture_type: " << roughness_u_texture_type << endl;

			if (roughness_u_texture_type == "constant") {
				float roughness_u = material.value("roughness_u", 0.1f);
				cout << "roughness_u: " << roughness_u << endl;

				m.coat_roughness_u = roughness_u;
			}
			else if (roughness_u_texture_type == "image") {
				string roughness_u_path = material.value("roughness_u_path", "");
				cout << "roughness_u_path: " << roughness_u_path << endl;

				texf.coat_roughness_uFile = roughness_u_path;
			}

			string roughness_v_texture_type = material.value("roughness_v_texture_type", "");
			shared_ptr<Texture> roughness_v_texture;
			cout << "roughness_v_texture_type: " << roughness_v_texture_type << endl;

			if (roughness_v_texture_type == "constant") {
				float roughness_v = material.value("roughness_v", 0.1f);
				cout << "roughness_v: " << roughness_v << endl;

				m.coat_roughness_v = roughness_v;
			}
			else if (roughness_v_texture_type == "image") {
				string roughness_v_path = material.value("roughness_v_path", "");
				cout << "roughness_v_path: " << roughness_v_path << endl;

				texf.coat_roughness_vFile = roughness_v_path;
			}

			float clear_coat = material.value("clear_coat", 1.0f);
			m.coat_weight = clear_coat;

			materialTable.insert({ name, {texf, m} });
		}
		else if (type == "metal_workflow") {
			string name = material.value("name", "");
			cout << "name: " << name << endl;

			string albedo_texture_type = material.value("albedo_texture_type", "");
			cout << "albedo_texture_type: " << albedo_texture_type << endl;

			TextureFile texf;
			Material m;
			m.type = MaterialType::MetalWorkflow;

			if (albedo_texture_type == "constant") {
				json albedo_j = material.value("albedo", json());
				vec3f albedo(albedo_j.at(0), albedo_j.at(1), albedo_j.at(2));
				cout << "albedo: " << albedo.x << "   " << albedo.y << "   " << albedo.z << "   " << endl;

				m.albedo = albedo;
			}
			else if (albedo_texture_type == "image") {
				string albedo_path = material.value("albedo_path", "");
				cout << "albedo_path: " << albedo_path << endl;

				texf.albedoFile = albedo_path;
			}

			string roughness_texture_type = material.value("roughness_texture_type", "");
			cout << "roughness_texture_type: " << roughness_texture_type << endl;

			if (roughness_texture_type == "constant") {
				float roughness = material.value("roughness", 0.1f);
				cout << "roughness: " << roughness << endl;

				m.roughness = roughness;
			}
			else if (roughness_texture_type == "image") {
				string roughness_path = material.value("roughness_path", "");
				cout << "roughness_path: " << roughness_path << endl;

				texf.roughnessFile = roughness_path;
			}

			string anisotropy_texture_type = material.value("anisotropy_texture_type", "");
			cout << "anisotropy_texture_type: " << anisotropy_texture_type << endl;

			if (anisotropy_texture_type == "constant") {
				float anisotropy = material.value("anisotropy", 0.0f);
				cout << "anisotropy: " << anisotropy << endl;

				m.anisotropy = anisotropy;
			}
			else if (anisotropy_texture_type == "image") {
				string anisotropy_path = material.value("anisotropy_path", "");
				cout << "anisotropy_path: " << anisotropy_path << endl;

				texf.anisotropyFile = anisotropy_path;
			}

			string metallic_texture_type = material.value("metallic_texture_type", "");
			cout << "metallic_texture_type: " << metallic_texture_type << endl;

			if (metallic_texture_type == "constant") {
				float metallic = material.value("metallic", 0.1f);
				cout << "metallic: " << metallic << endl;

				m.metallic = metallic;
			}
			else if (metallic_texture_type == "image") {
				string metallic_path = material.value("metallic_path", "");
				cout << "metallic_path: " << metallic_path << endl;

				texf.metallicFile = metallic_path;
			}

			string normal_texture_type = material.value("normal_texture_type", "");
			cout << "normal_texture_type: " << normal_texture_type << endl;
			if (normal_texture_type != "" && normal_texture_type == "image") {
				string normal_path = material.value("normal_path", "");
				cout << "normal_path: " << normal_path << endl;

				texf.normalFile = normal_path;
			}

			materialTable.insert({ name, {texf, m} });
		}

		cout << endl;
		i++;
	}

}

void SceneParser::ParseMedium(const json& data, Scene& scene) {
	int i = 0;
	for (auto iter = data.cbegin(); iter != data.cend(); ++iter) {
		json medium = data.at(i);
		string type = medium.value("type", "");
		cout << "type: " << type << endl;

		if (type == "homogeneous") {
			Medium mi;
			mi.type == MediumType::Homogeneous;

			string phase = medium.value("phase", "");
			cout << "phase: " << phase << endl;

			string name = medium.value("name", "");
			cout << "name: " << name << endl;

			json sigma_a_j = medium.value("sigma_a", json());
			vec3f sigma_a(sigma_a_j.at(0), sigma_a_j.at(1), sigma_a_j.at(2));
			cout << "sigma_a: " << sigma_a.x << "   " << sigma_a.y << "   " << sigma_a.z << "   " << endl;
			mi.sigma_a = sigma_a;

			json sigma_s_j = medium.value("sigma_s", json());
			vec3f sigma_s(sigma_s_j.at(0), sigma_s_j.at(1), sigma_s_j.at(2));
			cout << "sigma_s: " << sigma_s.x << "   " << sigma_s.y << "   " << sigma_s.z << "   " << endl;
			mi.sigma_s = sigma_s;

			float scale = medium.value("scale", 1.0f);
			cout << "scale: " << scale << endl;
			mi.scale = scale;

			if (phase == "isotropic") {
				mi.phase.type = PhaseType::Isotropic;
				scene.AddMedium(mi);
				int index = mediumTable.size();
				mediumTable.insert({ name, index });
			}
			else if (phase == "henyey_greenstein") {
				mi.phase.type == PhaseType::HenyeyGreenstein;
				json g_j = medium.value("g", json());
				vec3f g(g_j.at(0), g_j.at(1), g_j.at(2));
				mi.phase.g = g;
				cout << "g: " << g.x << "   " << g.y << "   " << g.z << "   " << endl;
				scene.AddMedium(mi);
				int index = mediumTable.size();
				mediumTable.insert({ name, index });
			}
		}

		i++;
		cout << endl;
	}
}
