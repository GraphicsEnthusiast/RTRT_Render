#include "Renderer.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
// our helper library for window handling
#include "glfWindow/GLFWindow.h"
#include <GL/gl.h>
#include "SceneParser.h"
#include "3rdParty/stb_image_write.h"

struct MyWindow : public GLFCameraWindow {
	MyWindow(const std::string& title, const Scene* scene, const SceneParser& parser)
		: GLFCameraWindow(title, parser.camera.from, parser.camera.at, parser.camera.up, parser.worldScale, parser.width, parser.height),
		renderer(scene) {
		renderer.SetCamera(parser.camera);
		camera_medium = parser.camera.medium;
		change_camera = false;
	}

	virtual void render() override {
		if (cameraFrame.modified) {
			renderer.SetCamera(Camera{ cameraFrame.get_from(),
									 cameraFrame.get_at(),
									 cameraFrame.get_up(),
									 camera_medium });
			cameraFrame.modified = false;
		}
		renderer.Render();
	}
	virtual void draw() override {
		renderer.DownloadPixels(pixels.data());
		if (fbTexture == 0) {
			glGenTextures(1, &fbTexture);
		}

		glBindTexture(GL_TEXTURE_2D, fbTexture);
		GLenum texFormat = GL_RGBA;
		GLenum texelType = GL_UNSIGNED_BYTE;
		glTexImage2D(GL_TEXTURE_2D, 0, texFormat, fbSize.x, fbSize.y, 0, GL_RGBA,
			texelType, pixels.data());

		glDisable(GL_LIGHTING);
		glColor3f(1, 1, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, fbTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glDisable(GL_DEPTH_TEST);

		glViewport(0, 0, fbSize.x, fbSize.y);
	}
	virtual void run() override {
		int width, height;
		glfwGetFramebufferSize(handle, &width, &height);
		resize(vec2i(width, height));

		glfwSetFramebufferSizeCallback(handle, glfwindow_reshape_cb);
		glfwSetKeyCallback(handle, glfwindow_key_cb);
		glfwSetMouseButtonCallback(handle, glfwindow_mouseButton_cb);
		glfwSetCursorPosCallback(handle, glfwindow_mouseMotion_cb);

		bool my_tool_active = true;

		while (!glfwWindowShouldClose(handle)) {
			render();
			draw();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL2_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::DockSpaceOverViewport();

			ImGui::Begin("Render Window", &my_tool_active, ImGuiWindowFlags_MenuBar);
			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("Open Scene")) {
						
					}
					if (ImGui::MenuItem("Save Image")) {
						stbi_flip_vertically_on_write(true);
						stbi_write_png("image.png", fbSize.x, fbSize.y, 4, pixels.data(), 0);
						std::cout << "save image" << std::endl;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			ImGui::Checkbox("Move Camera", &this->change_camera); ImGui::SameLine();
			ImGui::Checkbox("Denoising", &renderer.denoiserOn); ImGui::SameLine();
			ImGui::Checkbox("Progressive", &renderer.progressive); ImGui::SameLine();
			ImGui::Text("		Number of samples per pixel per frame = %d :", renderer.launchParams.numPixelSamples); ImGui::SameLine();
			if (ImGui::Button("Increace")) {
				renderer.launchParams.numPixelSamples++;
			}
			ImGui::SameLine();
			if (ImGui::Button("Decrease")) {
				renderer.launchParams.numPixelSamples--;
				if (renderer.launchParams.numPixelSamples < 1) {
					renderer.launchParams.numPixelSamples = 1;
				}
			}
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
			ImGui::Image((void*)(intptr_t)fbTexture, ImVec2(fbSize.x, fbSize.y), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();

			// Rendering
			ImGui::Render();

			ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

			// Update and Render additional Platform Windows
			// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
			//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
			if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			glfwMakeContextCurrent(handle);
			glfwSwapBuffers(handle);
			glfwPollEvents();
		}
	}
	virtual void resize(const vec2i& newSize) {
		fbSize = newSize;
		renderer.Resize(newSize);
		pixels.resize(newSize.x * newSize.y);
	}
	virtual void key(int key, int mods) {
		if (key == 'D' || key == 'd') {
			renderer.denoiserOn = !renderer.denoiserOn;
			std::cout << "denoising now " << (renderer.denoiserOn ? "ON" : "OFF") << std::endl;
		}
		if (key == 'A' || key == 'a') {
			renderer.progressive = !renderer.progressive;
			std::cout << "accumulation/progressive refinement now " << (renderer.progressive ? "ON" : "OFF") << std::endl;
		}
		if (key == ',') {
			renderer.launchParams.numPixelSamples
				= std::max(1, renderer.launchParams.numPixelSamples - 1);
			std::cout << "num samples/pixel now "
				<< renderer.launchParams.numPixelSamples << std::endl;
		}
		if (key == '.') {
			renderer.launchParams.numPixelSamples
				= std::max(1, renderer.launchParams.numPixelSamples + 1);
			std::cout << "num samples/pixel now "
				<< renderer.launchParams.numPixelSamples << std::endl;
		}
	}

	vec2i fbSize;
	GLuint fbTexture{ 0 };
	Renderer renderer;
	std::vector<uint32_t> pixels;
	int camera_medium;
};

extern "C" int main(int ac, char** av) {
	try {
// 		Light light;
// 		light.medium = -1;
// 		light.position = vec3f(0.5f, 0.25f, 0.0f);
// 		light.radius = 0.1f;
// 		light.radiance = vec3f(15.0f);
// 		scene.AddLight(light);
// 
// 		light.position = vec3f(1.0f, 0.5f, 0.0f);
// 		light.radius = 0.08f;
// 		light.radiance = vec3f(0.0f, 15.0f, 0.0f);
// 		scene.AddLight(light);
// 
// 		light.position = vec3f(-0.5f, 0.25f, 0.0f);
// 		light.radius = 0.1f;
// 		light.radiance = vec3f(15.0f, 0.0f, 0.0f);
// 		scene.AddLight(light);
		Scene scene;
		SceneParser parser;
		bool is_succeed;
		parser.LoadFromJson("../../models/test.json", scene, is_succeed);

		MyWindow* window = new MyWindow("OptiXRender", &scene, parser);
//      window->enableFlyMode();
		window->enableInspectMode();
		window->run();
	}
	catch (std::runtime_error& e) {
		std::cout << GDT_TERMINAL_RED << "FATAL ERROR: " << e.what()
			<< GDT_TERMINAL_DEFAULT << std::endl;
		exit(1);
	}

	return 0;
}