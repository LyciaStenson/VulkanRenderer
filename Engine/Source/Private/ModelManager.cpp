#include <ModelManager.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <stb_image.h>

#include <Transform.h>
#include <Model.h>
#include <Mesh.h>
#include <MeshPrimitive.h>
#include <MeshInstance.h>
#include <VulkanTexture.h>

using namespace VulkanRenderer;

ModelManager::ModelManager(VulkanDevice* device, VkDescriptorSetLayout uniformDescriptorSetLayout, VkDescriptorSetLayout materialDescriptorSetLayout, VkDescriptorPool descriptorPool)
	: device(device), uniformDescriptorSetLayout(uniformDescriptorSetLayout), materialDescriptorSetLayout(materialDescriptorSetLayout), descriptorPool(descriptorPool)
{
	fallbackTexture = CreateFallbackTexture(glm::vec4(1.0f));
}

ModelManager::~ModelManager()
{

}

std::shared_ptr<Model> ModelManager::GetModel(const std::string& name)
{
	return models[name];
}

std::shared_ptr<Model> ModelManager::LoadModel(const std::string& name, const std::filesystem::path& path)
{
	auto it = models.find(name);
	if (it != models.end())
	{
		return it->second;
	}
	
	fastgltf::Parser parser;

	auto data = fastgltf::GltfDataBuffer::FromPath(path);
	if (data.error() != fastgltf::Error::None)
	{
		std::cout << "fastgltf get buffer error: " << fastgltf::getErrorMessage(data.error()) << std::endl;
		return false;
	}
	
	auto asset = parser.loadGltfBinary(data.get(), path.parent_path(), fastgltf::Options::None);
	if (asset.error() != fastgltf::Error::None)
	{
		std::cout << "fastgltf get data error: " << fastgltf::getErrorMessage(asset.error()) << std::endl;
		return false;
	}
	
	std::shared_ptr<Model> model = std::make_shared<Model>();
	model->name = name;
	model->gltfAsset = std::move(asset.get());

	auto& gltfAsset = model->gltfAsset;

	LoadTextures(model);
	
	for (size_t meshIndex = 0; meshIndex < gltfAsset.meshes.size(); ++meshIndex)
	{
		const fastgltf::Mesh& gltfMesh = gltfAsset.meshes[meshIndex];
		
		auto mesh = std::make_shared<Mesh>(device, uniformDescriptorSetLayout);

		for (size_t primitiveIndex = 0; primitiveIndex < gltfMesh.primitives.size(); ++primitiveIndex)
		{
			const fastgltf::Primitive& primitive = gltfMesh.primitives[primitiveIndex];
			
			auto positionIt = primitive.findAttribute("POSITION");
			auto normalIt = primitive.findAttribute("NORMAL");
			auto tangentIt = primitive.findAttribute("TANGENT");
			
			MeshPrimitiveInfo primitiveInfo{};
			
			if (positionIt != primitive.attributes.end())
			{
				const auto& positionAccessor = gltfAsset.accessors[positionIt->accessorIndex];
				primitiveInfo.vertices.resize(positionAccessor.count);

				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(gltfAsset, positionAccessor, [&](fastgltf::math::fvec3 position, std::size_t verticeIndex)
					{
						primitiveInfo.vertices[verticeIndex].position = glm::vec3(position.x(), position.y(), position.z());
					});
			}

			if (normalIt != primitive.attributes.end())
			{
				auto& normalAccessor = gltfAsset.accessors[normalIt->accessorIndex];
				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(gltfAsset, normalAccessor, [&](fastgltf::math::fvec3 normal, std::size_t verticeIndex)
					{
						//primitiveInfo.vertices[verticeIndex].normal = glm::vec3(normal.x(), normal.y(), normal.z());
					});
			}

			if (tangentIt != primitive.attributes.end())
			{
				auto& tangentAccessor = gltfAsset.accessors[tangentIt->accessorIndex];
				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(gltfAsset, tangentAccessor, [&](fastgltf::math::fvec4 tangent, std::size_t verticeIndex)
					{
						//primitiveInfo.vertices[verticeIndex].tangent = glm::vec4(tangent.x(), tangent.y(), tangent.z(), tangent.w());
					});
			}

			if (primitive.indicesAccessor.has_value())
			{
				const auto& indexAccessor = gltfAsset.accessors[primitive.indicesAccessor.value()];
				fastgltf::iterateAccessorWithIndex<std::uint16_t>(gltfAsset, indexAccessor, [&](std::uint16_t index, std::size_t verticeIndex)
					{
						primitiveInfo.indices.push_back(index);
					});
			}

			std::size_t baseColorTexcoordIndex = 0;
			std::size_t metallicRoughnessTexcoordIndex = 0;
			std::size_t normalTexcoordIndex = 0;
			if (primitive.materialIndex.has_value())
			{
				auto& material = gltfAsset.materials[primitive.materialIndex.value()];

				primitiveInfo.doubleSided = material.doubleSided;

				//auto& baseColorFactor = material.pbrData.baseColorFactor;
				//meshPrimitive.material.baseColorFactor = glm::vec4(baseColorFactor.x(), baseColorFactor.y(), baseColorFactor.z(), baseColorFactor.w());
				//meshPrimitive.material.hasBaseColorTexture = false;
				auto& baseColorTexture = material.pbrData.baseColorTexture;
				if (baseColorTexture.has_value())
				{
					auto& texture = gltfAsset.textures[baseColorTexture->textureIndex];
					if (texture.imageIndex.has_value())
					{
						//meshPrimitive.material.baseColorTexture = texturesMap[modelName][texture.imageIndex.value()].id;
						//meshPrimitive.material.hasBaseColorTexture = true;
						if (baseColorTexture->transform && baseColorTexture->transform->texCoordIndex.has_value())
						{
							baseColorTexcoordIndex = baseColorTexture->transform->texCoordIndex.value();
						}
						else
						{
							baseColorTexcoordIndex = material.pbrData.baseColorTexture->texCoordIndex;
						}
					}
				}

				auto& metallicRoughnessTexture = material.pbrData.metallicRoughnessTexture;
				//meshPrimitive.material.metallicFactor = material.pbrData.metallicFactor;
				//meshPrimitive.material.roughnessFactor = material.pbrData.roughnessFactor;
				//meshPrimitive.material.hasMetallicRoughnessTexture = false;
				if (metallicRoughnessTexture.has_value())
				{
					auto& texture = gltfAsset.textures[metallicRoughnessTexture->textureIndex];
					if (texture.imageIndex.has_value())
					{
						//meshPrimitive.material.metallicRoughnessTexture = texturesMap[modelName][texture.imageIndex.value()].id;
						//meshPrimitive.material.hasMetallicRoughnessTexture = true;
						if (metallicRoughnessTexture->transform && metallicRoughnessTexture->transform->texCoordIndex.has_value())
						{
							metallicRoughnessTexcoordIndex = metallicRoughnessTexture->transform->texCoordIndex.value();
						}
						else
						{
							metallicRoughnessTexcoordIndex = material.pbrData.baseColorTexture->texCoordIndex;
						}
					}
				}

				auto& normalTexture = material.normalTexture;
				//meshPrimitive.material.hasNormalTexture = false;
				if (normalTexture.has_value())
				{
					auto& texture = gltfAsset.textures[normalTexture->textureIndex];
					if (texture.imageIndex.has_value())
					{
						//meshPrimitive.material.normalTexture = texturesMap[modelName][texture.imageIndex.value()].id;
						//meshPrimitive.material.hasNormalTexture = true;
						if (normalTexture->transform && normalTexture->transform->texCoordIndex.has_value())
						{
							normalTexcoordIndex = normalTexture->transform->texCoordIndex.value();
						}
						else
						{
							normalTexcoordIndex = material.normalTexture->texCoordIndex;
						}
					}
				}
			}

			auto baseColorTexcoordAttribute = std::string("TEXCOORD_") + std::to_string(baseColorTexcoordIndex);
			if (const auto* texcoord = primitive.findAttribute(baseColorTexcoordAttribute); texcoord != primitive.attributes.end())
			{
				auto& texcoordAccessor = gltfAsset.accessors[texcoord->accessorIndex];
				if (texcoordAccessor.bufferViewIndex.has_value())
				{
					fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(gltfAsset, texcoordAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx)
						{
							//primitiveInfo.vertices[idx].texCoord = glm::vec2(uv.x(), uv.y());
							primitiveInfo.vertices[idx].baseColorTexCoord = glm::vec2(uv.x(), uv.y());
						});
				}
			}

			auto metallicRoughnessTexcoordAttribute = std::string("TEXCOORD_") + std::to_string(metallicRoughnessTexcoordIndex);
			if (const auto* texcoord = primitive.findAttribute(metallicRoughnessTexcoordAttribute); texcoord != primitive.attributes.end())
			{
				auto& texcoordAccessor = gltfAsset.accessors[texcoord->accessorIndex];
				if (texcoordAccessor.bufferViewIndex.has_value())
				{
					fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(gltfAsset, texcoordAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx)
						{
							primitiveInfo.vertices[idx].metallicRoughnessTexCoord = glm::vec2(uv.x(), uv.y());
						});
				}
			}

			auto normalTexcoordAttribute = std::string("TEXCOORD_") + std::to_string(normalTexcoordIndex);
			if (const auto* texcoord = primitive.findAttribute(normalTexcoordAttribute); texcoord != primitive.attributes.end())
			{
				auto& texcoordAccessor = gltfAsset.accessors[texcoord->accessorIndex];
				if (texcoordAccessor.bufferViewIndex.has_value())
				{
					fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(gltfAsset, texcoordAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx)
						{
							primitiveInfo.vertices[idx].normalTexCoord = glm::vec2(uv.x(), uv.y());
						});
				}
			}
			
			primitiveInfo.baseColorTexture = fallbackTexture;
			primitiveInfo.metallicRoughnessTexture = fallbackTexture;
			primitiveInfo.normalTexture = fallbackTexture;

			if (primitive.materialIndex.has_value())
			{
				const auto& material = gltfAsset.materials[primitive.materialIndex.value()];

				if (material.pbrData.baseColorTexture.has_value())
				{
					const auto& texture = gltfAsset.textures[material.pbrData.baseColorTexture->textureIndex];
					if (texture.imageIndex.has_value())
					{
						auto imageIndex = texture.imageIndex.value();
						primitiveInfo.baseColorTexture = model->textures[imageIndex];
					}
				}
				if (material.pbrData.metallicRoughnessTexture.has_value())
				{
					const auto& texture = gltfAsset.textures[material.pbrData.metallicRoughnessTexture->textureIndex];
					if (texture.imageIndex.has_value())
					{
						auto imageIndex = texture.imageIndex.value();
						primitiveInfo.metallicRoughnessTexture = model->textures[imageIndex];
					}
				}
				if (material.normalTexture.has_value())
				{
					const auto& texture = gltfAsset.textures[material.normalTexture->textureIndex];
					if (texture.imageIndex.has_value())
					{
						auto imageIndex = texture.imageIndex.value();
						primitiveInfo.normalTexture = model->textures[imageIndex];
					}
				}
			}
			
			auto meshPrimitive = std::make_unique<MeshPrimitive>(device, materialDescriptorSetLayout, descriptorPool, primitiveInfo);
			mesh->AddPrimitive(std::move(meshPrimitive));
		}
		
		model->meshes.push_back(mesh);
	}
	
	models[name] = model;

	return model;
}

void ModelManager::LoadTextures(std::shared_ptr<Model>& model)
{
	fastgltf::Asset& asset = model->gltfAsset;
	
	std::unordered_map<size_t, ImageData> decodedImages;
	
	for (size_t imageIndex = 0; imageIndex < asset.images.size(); ++imageIndex)
	{
		auto& image = asset.images[imageIndex];

		ImageData data;

		if (DecodeImage(asset, image, data.pixels, data.width, data.height, data.channels))
			decodedImages[imageIndex] = std::move(data);
		else
			std::cerr << "Failed to decode image at index " << imageIndex << std::endl;
	}

	model->textures.clear();
	
	for (size_t textureIndex = 0; textureIndex < asset.textures.size(); ++textureIndex)
	{
		const auto& texture = asset.textures[textureIndex];

		auto imageIndex = texture.imageIndex;

		if (!imageIndex.has_value() || decodedImages.count(imageIndex.value()) == 0)
		{
			std::cerr << "Texture " << textureIndex << " references a missing image." << std::endl;
			continue;
		}

		bool sRGB = false;
		for (auto& material : asset.materials)
		{
			if (material.pbrData.baseColorTexture.has_value() && material.pbrData.baseColorTexture->textureIndex == textureIndex)
			{
				sRGB = true;
				break;
			}
		}

		auto& imageData = decodedImages[imageIndex.value()];

		auto vulkanTexture = std::make_shared<VulkanTexture>(device, imageData.pixels.data(), imageData.width, imageData.height, sRGB);
		model->textures[textureIndex] = vulkanTexture;
	}
}

bool ModelManager::DecodeImage(const fastgltf::Asset& asset, const fastgltf::Image& image, std::vector<uint8_t>& outPixels, int& outWidth, int& outHeight, int& outChannels)
{
	bool decoded = false;

	std::visit(fastgltf::visitor
		{
			[&](const fastgltf::sources::URI& filePath)
			{
				std::cout << "External GLTF textures not yet supported. Please use GLB models." << std::endl;
			},
			[&](const fastgltf::sources::Array& vector)
			{
				std::cout << "GLTF textures not yet supported. Please use GLB models." << std::endl;
			},
			[&](const fastgltf::sources::BufferView& view)
			{
				if (view.bufferViewIndex >= asset.bufferViews.size())
				{
					std::cerr << "Invalid buffer view index in image." << std::endl;
					return;
				}
				
				auto& bufferView = asset.bufferViews[view.bufferViewIndex];
				auto& buffer = asset.buffers[bufferView.bufferIndex];

				stbi_set_flip_vertically_on_load(false);

				std::visit(fastgltf::visitor
					{
						[&](const fastgltf::sources::Array& vector)
						{
							unsigned char* pixels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(vector.bytes.data() + bufferView.byteOffset),
																		static_cast<int>(bufferView.byteLength), &outWidth, &outHeight, &outChannels, 4);
							if (pixels)
							{
								outPixels.assign(pixels, pixels + (outWidth * outHeight * 4));

								outChannels = 4;

								stbi_image_free(pixels);

								decoded = true;
							}
							else
							{
								std::cerr << "STB Image failed to decode image." << std::endl;
							}
						},
						[](auto& arg) {}
					}, buffer.data);
			},
			[](auto& arg) {}
		}, image.data);
	
	return decoded;
}

std::shared_ptr<VulkanTexture> ModelManager::CreateFallbackTexture(glm::vec4 color)
{
	uint8_t pixel[4] =
	{
		static_cast<uint8_t>(color.r * 255),
		static_cast<uint8_t>(color.g * 255),
		static_cast<uint8_t>(color.b * 255),
		static_cast<uint8_t>(color.a * 255)
	};

	return std::make_shared<VulkanTexture>(device, pixel, 1, 1, VK_FORMAT_R8G8B8A8_UNORM);
}