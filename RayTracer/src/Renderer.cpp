#include "Renderer.h"

#include "Walnut/Random.h"
#include "Ray.h"
#include "Camera.h"
#include "Scene.h"
#include <algorithm>
#include <execution>

namespace Utils
{
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);
		return (a << 24) | (b << 16) | (g << 8) | r;
	}
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;
		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

	delete[] m_AccumulateData;
	m_AccumulateData = new glm::vec4[width * height];

	m_ImageHorizontalIter.resize(width);
	m_ImageVerticalIter.resize(height);
	for (uint32_t i = 0; i < width; i++)  m_ImageHorizontalIter[i] = i;
	for (uint32_t i = 0; i < height; i++) m_ImageVerticalIter[i] = i;
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;

	m_ImageWidth = m_FinalImage->GetWidth();
	m_ImageHeight = m_FinalImage->GetHeight();

	if (m_FrameIndex == 1)
		memset(m_AccumulateData, 0, m_ImageWidth * m_ImageHeight * sizeof(glm::vec4));

	float invFrameIndex = 1.0f / (float)m_FrameIndex;

	std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
		[this, invFrameIndex](uint32_t y)
		{
			std::for_each(std::execution::seq, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
				[this, y, invFrameIndex](uint32_t x)
				{
					glm::vec4 color = RayGen(x, y);
					m_AccumulateData[x + y * m_ImageWidth] += color;

					glm::vec4 accumulateColor = m_AccumulateData[x + y * m_ImageWidth] * invFrameIndex;
					accumulateColor = glm::clamp(accumulateColor, glm::vec4(0.0f), glm::vec4(1.0f));
					m_ImageData[x + y * m_ImageWidth] = Utils::ConvertToRGBA(accumulateColor);
				});
		});

	m_FinalImage->SetData(m_ImageData);

	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}

glm::vec4 Renderer::RayGen(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_ImageWidth];

	glm::vec3 light(0.0f);
	glm::vec3 throughput(1.0f);

	int bounces = 5;
	for (int i = 0; i < bounces; i++)
	{
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f)
		{
			light += m_Settings.SkyColor * throughput;
			break;
		}

		const Sphere& sphere = m_ActiveScene->Sphere[payload.ObjectIndex];
		const Material& material = m_ActiveScene->Material[sphere.MaterialIndex];

		glm::vec3 albedoTint = glm::mix(glm::vec3(1.0f), material.Albedo, material.Metallic);
		throughput *= albedoTint * material.Albedo;

		light += material.GetEmission();

		// Hard cutoff — skip rays that can't contribute
		if (glm::dot(throughput, throughput) < 1e-6f)
			break;

		// Russian Roulette
		float p = glm::max(throughput.r, glm::max(throughput.g, throughput.b));
		if (Walnut::Random::Float() > p)
			break;
		throughput /= p;

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;

		glm::vec3 diffuseDir = glm::normalize(payload.WorldNormal + Walnut::Random::InUnitSphere());
		glm::vec3 specularDir = glm::reflect(ray.Direction,
			payload.WorldNormal + material.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f));
		ray.Direction = glm::normalize(glm::mix(diffuseDir, specularDir, material.Metallic));
	}

	return glm::vec4(light, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	int   closestSphere = -1;
	float hitDistance = FLT_MAX;

	for (size_t i = 0; i < m_ActiveScene->Sphere.size(); i++)
	{
		const Sphere& sphere = m_ActiveScene->Sphere[i];
		glm::vec3 origin = ray.Origin - sphere.Position;

		float b = glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;
		float discriminant = b * b - c;
		if (discriminant < 0.0f) continue;

		float closestT = -b - glm::sqrt(discriminant);
		if (closestT > 0.0f && closestT < hitDistance)
		{
			hitDistance = closestT;
			closestSphere = (int)i;
		}
	}

	if (closestSphere < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	Renderer::HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Sphere& closestSphere = m_ActiveScene->Sphere[objectIndex];
	glm::vec3 origin = ray.Origin - closestSphere.Position;
	payload.WorldPosition = origin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);
	payload.WorldPosition += closestSphere.Position;

	return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
}