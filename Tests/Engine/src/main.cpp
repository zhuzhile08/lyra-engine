#include <Lyra/Lyra.h>
#include <Common/Common.h>
#include <Common/FileSystem.h>
#include <Common/Benchmark.h>

#include <Graphics/VulkanRenderSystem.h>
#include <Graphics/Renderer.h>
#include <Graphics/Window.h>
#include <Graphics/Material.h>
#include <Graphics/Texture.h>
#include <Graphics/Mesh.h>

#include <ECS/Entity.h>
#include <ECS/Components/Transform.h>
#include <ECS/Components/Camera.h>
#include <ECS/Components/MeshRenderer.h>

#include <Resource/ResourceSystem.h>

#include <Input/InputSystem.h>

struct CameraScript : lyra::BasicScript {
	static constexpr lyra::float32 speed = 10.0f;
	static constexpr lyra::float32 sensitivity = 5.0f;
	
	void init(void) {
		transform = &entity->component<lyra::Transform>();
		
		transform->translation = { 0.0f, 2.0f, 2.0f };
		transform->lookAt({0.0f, 0.0f, 0.0f});
	}
	
	void update(void) {
		transform->rotate(transform->globalUp(), lyra::input::mouseDelta().x / lyra::renderer::drawWidth() * sensitivity);
		transform->rotate(transform->left(), -lyra::input::mouseDelta().y / lyra::renderer::drawHeight() * sensitivity);

		if (lyra::input::keyboard(lyra::input::KeyType::w).held) {
			transform->translation += (transform->forward() * speed * lyra::renderer::deltaTime());
		} if (lyra::input::keyboard(lyra::input::KeyType::s).held) {
			transform->translation += (-transform->forward() * speed * lyra::renderer::deltaTime());
		} if (lyra::input::keyboard(lyra::input::KeyType::a).held) {
			transform->translation += (transform->left() * speed * lyra::renderer::deltaTime());
		} if (lyra::input::keyboard(lyra::input::KeyType::d).held) {
			transform->translation += (-transform->left() * speed * lyra::renderer::deltaTime());
		}
	}
	
	lyra::Transform* transform;
};

int main(int argc, char* argv[]) {
	lyra::init(lyra::InitFlags::all, { argc, argv });

	lyra::Entity sceneRoot;
	auto& c = sceneRoot
		.addComponent<lyra::Transform>()
		.emplace("Camera")
			.addComponent<lyra::Transform>()
			.addComponent<CameraScript>()
			.addComponent<lyra::Camera>();
	
	lyra::Material material(
		lyra::Color(),
		{ &lyra::resource::texture("img/skybox.png") }
	);

	lyra::Mesh quad(
		{
			{ { -5.0f, -5.0f, 0.0f }, glm::vec3(1), { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
			{ { 5.0f, -5.0f, 0.0f }, glm::vec3(1), { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
			{ { 5.0f, 5.0f, 0.0f }, glm::vec3(1), { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
   			{ { -5.0f, 5.0f, 0.0f }, glm::vec3(1), { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } }
		},
		{
			0, 1, 2, 2, 3, 0
		}
	);

	sceneRoot.emplace("MeshRenderer")
		.addComponent<lyra::MeshRenderer>(quad, material);
		//.addComponent<lyra::MeshRenderer>(lyra::resource::mesh("mesh/viking_room.obj", 0), material)
		//.addComponent<lyra::MeshRenderer>(lyra::resource::mesh("mesh/cube.fbx", 0), material);

	lyra::renderer::setScene(sceneRoot);

	while (!lyra::input::quit()) {
		lyra::input::update();
		lyra::ecs::update();

		lyra::renderer::beginFrame();
		
		lyra::renderer::draw();
		
		lyra::renderer::endFrame();
	}

	lyra::quit();

	return 0;
}
