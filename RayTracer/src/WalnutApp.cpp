#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "glm/gtc/type_ptr.hpp"

    
#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"

using namespace Walnut;
using namespace std;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		:m_Camera(45.0f, 1.0f, 100.0f)
	{
		Material& Sphere_1 = m_Scene.Material.emplace_back();
		Sphere_1.Albedo = { 0.93725490f, 0.91372549f, 0.87843137f };
		Sphere_1.Roughness = { 1.0f};


		Material& Sphere_2 = m_Scene.Material.emplace_back();
		Sphere_2.Albedo = { 0.05882353f, 0.61960784f, 0.60000000f };
		Sphere_2.Roughness = { 1.0f };

		Material& Sphere_3 = m_Scene.Material.emplace_back();
		Sphere_3.Albedo = { 1.0f, 0.443f, 0.349f };
		Sphere_3.Roughness = { 1.0f };
		Sphere_3.EmissionColor = Sphere_3.Albedo;
		Sphere_3.EmissionStrength = 2.0f;



		{
			Sphere sphere;
			sphere.Position = { 0.0f,0.0f,0.0f };
			sphere.Radius = { 1.0f };
			sphere.MaterialIndex = 0;
			m_Scene.Sphere.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { 0.0f,-101.7f,4.0f };
			sphere.Radius = { 100.5f };
			sphere.MaterialIndex = 1;
			m_Scene.Sphere.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { 4.0f,0.0f,0.0f };
			sphere.Radius = { 1.0f };
			sphere.MaterialIndex = 2;
			m_Scene.Sphere.push_back(sphere);
		}

	}

	virtual void OnUpdate(float ts) override
	{
		m_Camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
			if (ImGui::ColorEdit3("Sky Color", glm::value_ptr(m_Renderer.GetSettings().SkyColor))) 
			{
				m_Renderer.ResetFrameIndex();			
			}
		if(ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate))
		{
			m_Renderer.ResetFrameIndex();
		}

		if (ImGui::Button("Reset")) {
		m_Renderer.ResetFrameIndex();
		};
		ImGui::End();

		ImGui::Begin("Scene");
		for (int i = 0; i < m_Scene.Sphere.size(); i++) 
		{
			ImGui::PushID(i);

			Sphere& sphere = m_Scene.Sphere[i];

			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
			ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0.0f, (int)m_Scene.Material.size() - 1);

			ImGui::Separator();
			ImGui::PopID();

		}
		for (size_t i = 0; i < m_Scene.Material.size(); i++) 
		{
			ImGui::PushID(i);
			Material& material = m_Scene.Material[i];
			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			ImGui::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Metallic", &material.Metallic, 0.05f, 0.0f, 1.0f);
			ImGui::ColorEdit3("Emission_Color", glm::value_ptr(material.EmissionColor));
			ImGui::DragFloat("Emission_Strength", &material.EmissionStrength, 0.05f, 0.0f, FLT_MAX);
			if (ImGui::Checkbox("Emission Enabled", &material.EmissionEnabled))
				m_Renderer.ResetFrameIndex();

			ImGui::Separator();
			ImGui::PopID();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		ImGui::Text("LastRender: %.3fms", m_LastRenderTime);

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;
		auto image = m_Renderer.GetFinalImage();
		if (image)
		{
			ImGui::Image(image->GetDescriptorSet(), {(float)image->GetWidth(),(float)image->GetHeight() }, ImVec2(0,1),ImVec2(1,0));
		}
		ImGui::End();
		ImGui::PopStyleVar();
		Render();
	}

	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene,m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();   
	}

private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	float m_LastRenderTime = 0.0f;
};


Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "RayTracer";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}