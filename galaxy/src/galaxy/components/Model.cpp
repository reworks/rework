///
/// Model.cpp
/// galaxy
///
/// Refer to LICENSE.txt for more details.
///

#include <execution>
#include <mutex>

#include "galaxy/core/ServiceLocator.hpp"
#include "galaxy/fs/FileSystem.hpp"
#include "galaxy/scripting/JSONUtils.hpp"

#include "Model.hpp"

namespace galaxy
{
	namespace components
	{
		Model::Model() noexcept
		    : Serializable {this}, m_file {""}
		{
		}

		Model::Model(const nlohmann::json& json)
		    : Serializable {this}
		{
			deserialize(json);
		}

		Model::Model(Model&& m) noexcept
		    : Serializable {this}
		{
			this->m_file   = std::move(m.m_file);
			this->m_meshes = std::move(m.m_meshes);
		}

		Model& Model::operator=(Model&& m) noexcept
		{
			if (this != &m)
			{
				this->m_file   = std::move(m.m_file);
				this->m_meshes = std::move(m.m_meshes);
			}

			return *this;
		}

		Model::~Model() noexcept
		{
			m_meshes.clear();
		}

		const bool Model::load(std::string_view file)
		{
			bool result    = false;
			const auto abs = SL_HANDLE.vfs()->absolute(file);
			if (abs != std::nullopt)
			{
				m_file = file;

				static constexpr const auto flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_FindInvalidData | aiProcess_SortByPType | aiProcess_RemoveRedundantMaterials | aiProcess_ImproveCacheLocality | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace;

				Assimp::Importer importer;
				auto scene = importer.ReadFile(abs.value(), flags);
				if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
				{
					GALAXY_LOG(GALAXY_ERROR, "Failed to import model: {0}, because: {1}.", file, importer.GetErrorString());
				}
				else
				{
					parse_node(scene->mRootNode, scene);
					result = true;
				}
			}
			else
			{
				GALAXY_LOG(GALAXY_ERROR, "Failed to find model on disk: {0}.", file);
			}

			return result;
		}

		void Model::create()
		{
			for (auto& mesh : m_meshes)
			{
				mesh.optimize();
				mesh.create();
			}
		}

		void Model::recreate()
		{
			for (auto& mesh : m_meshes)
			{
				mesh.create();
			}
		}

		std::vector<graphics::Mesh>& Model::get_meshes() noexcept
		{
			return m_meshes;
		}

		void Model::parse_node(aiNode* node, const aiScene* scene) noexcept
		{
			m_meshes.reserve(node->mNumMeshes);
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				auto ai_mesh = scene->mMeshes[node->mMeshes[i]];
				auto mesh    = std::move(parse_mesh(ai_mesh, scene));

				m_meshes.emplace_back(std::move(mesh));
			}

			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				parse_node(node->mChildren[i], scene);
			}
		}

		graphics::Mesh Model::parse_mesh(aiMesh* mesh, const aiScene* scene) noexcept
		{
			graphics::Mesh output;

			// Free up memory after, since meshes can be quite large.
			{
				std::vector<unsigned int> count;
				count.resize(mesh->mNumVertices, 0);
				std::iota(count.begin(), count.end(), 0);

				// Use a vector of sequential numbers allows us to parse the verticies in parallel.
				std::mutex mutex;
				std::for_each(std::execution::par, count.begin(), count.end(), [&](const auto& i) {
					graphics::Vertex3D vertex;

					if (mesh->HasPositions())
					{
						vertex.m_pos.x = mesh->mVertices[i].x;
						vertex.m_pos.y = mesh->mVertices[i].y;
						vertex.m_pos.z = mesh->mVertices[i].z;
					}

					if (mesh->HasNormals())
					{
						vertex.m_normal.x = mesh->mNormals[i].x;
						vertex.m_normal.y = mesh->mNormals[i].y;
						vertex.m_normal.z = mesh->mNormals[i].z;
					}

					if (mesh->HasTextureCoords(0))
					{
						vertex.m_texels.x = mesh->mTextureCoords[0][i].x;
						vertex.m_texels.y = mesh->mTextureCoords[0][i].y;
					}
					else
					{
						vertex.m_texels.x = 0.0f;
						vertex.m_texels.y = 0.0f;
					}

					if (mesh->HasTangentsAndBitangents())
					{
						vertex.m_tangents.x = mesh->mTangents[i].x;
						vertex.m_tangents.y = mesh->mTangents[i].y;
						vertex.m_tangents.z = mesh->mTangents[i].z;

						vertex.m_bitangents.x = mesh->mBitangents[i].x;
						vertex.m_bitangents.y = mesh->mBitangents[i].y;
						vertex.m_bitangents.z = mesh->mBitangents[i].z;
					}

					vertex.set_index(i);

					// Only shared resource.
					mutex.lock();
					output.m_verticies.emplace_back(vertex);
					mutex.unlock();
				});

				// Sort verticies afterwards to ensure correct order.
				// No need for stable sort, each index is guaranteed to be different.
				std::sort(std::execution::par_unseq, output.m_verticies.begin(), output.m_verticies.end(), [&](const auto& left, const auto& right) {
					return left.get_index() < right.get_index();
				});
			}

			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				const auto& face = mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
				{
					output.m_indicies.push_back(face.mIndices[j]);
				}
			}

			if (mesh->mMaterialIndex >= 0)
			{
				auto* material     = scene->mMaterials[mesh->mMaterialIndex];
				auto* materialbook = SL_HANDLE.materialbook();

				light::Material* light_mat;
				if (materialbook->has(material->GetName().C_Str()))
				{
					light_mat = materialbook->get(material->GetName().C_Str());
				}
				else
				{
					light_mat = materialbook->create(material->GetName().C_Str());
				}

				if (light_mat->m_shininess < 0.0f)
				{
					if (material->Get(AI_MATKEY_SHININESS, light_mat->m_shininess) != aiReturn_SUCCESS)
					{
						light_mat->m_shininess = 32.0f;
					}
				}

				parse_material_texture(material, aiTextureType_DIFFUSE, light_mat);
				parse_material_texture(material, aiTextureType_SPECULAR, light_mat);
				parse_material_texture(material, aiTextureType_HEIGHT, light_mat); // Normal map.

				output.m_material_id = material->GetName().C_Str();
			}

			return output;
		}

		void Model::parse_material_texture(aiMaterial* mat, aiTextureType type, light::Material* material)
		{
			aiString str;
			mat->GetTexture(type, 0, &str);

			switch (type)
			{
				case aiTextureType_DIFFUSE:
					if (!material->m_diffuse.is_loaded())
					{
						const auto filename = std::filesystem::path(str.C_Str()).filename().string();
						const auto abs      = SL_HANDLE.vfs()->absolute(filename);

						if (abs != std::nullopt)
						{
							material->m_diffuse.load(abs.value());
						}
						else
						{
							GALAXY_LOG(GALAXY_WARNING, "Failed to load mesh diffuse texture: {0}.", filename);
						}
					}
					break;

				case aiTextureType_SPECULAR:
					if (!material->m_specular.is_loaded())
					{
						const auto filename = std::filesystem::path(str.C_Str()).filename().string();
						const auto abs      = SL_HANDLE.vfs()->absolute(filename);

						if (abs != std::nullopt)
						{
							material->m_specular.load(abs.value());
						}
						else
						{
							GALAXY_LOG(GALAXY_WARNING, "Failed to load mesh specular texture: {0}.", filename);
						}
					}
					break;

				case aiTextureType_HEIGHT:
					if (!material->m_normal.is_loaded())
					{
						const auto filename = std::filesystem::path(str.C_Str()).filename().string();
						const auto abs      = SL_HANDLE.vfs()->absolute(filename);

						if (abs != std::nullopt)
						{
							material->m_normal.load(abs.value());
						}
						else
						{
							GALAXY_LOG(GALAXY_WARNING, "Failed to load mesh normalmap texture: {0}.", filename);
						}
					}
					break;
			}
		}

		nlohmann::json Model::serialize()
		{
			nlohmann::json json = "{}"_json;
			json["file"]        = m_file;

			return json;
		}

		void Model::deserialize(const nlohmann::json& json)
		{
			m_meshes.clear();

			if (!load(json.at("file")))
			{
				GALAXY_LOG(GALAXY_ERROR, "Failed to load model from deserialized json.");
			}
			else
			{
				create();
			}
		}
	} // namespace components
} // namespace galaxy