#ifndef __VKPP_SAMPLE_MULTIPIPELINE_H__
#define __VKPP_SAMPLE_MULTIPIPELINE_H__



#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <tinyobjloader/tiny_obj_loader.h>

#include <Base/ExampleBase.h>
#include <Window/Window.h>
#include <Window/WindowEvent.h>



namespace vkpp::sample
{



struct VertexData
{
    glm::vec3 inPosition;
    glm::vec3 inColor;
    glm::vec2 texCoord;
    glm::vec3 normal;

    constexpr static vkpp::VertexInputBindingDescription GetBindingDescription(void)
    {
        return {
            0,                                  // binding
            sizeof(VertexData),                 // stride
            vkpp::VertexInputRate::eVertex      // input rate
        };
    }

    constexpr static decltype(auto) GetAttributeDescriptions(void)
    {
        constexpr std::array<vkpp::VertexInputAttributeDescription, 4> lInputAttributeDescriptions
        {{
            // Location 0: Position
            {
                0,                                  // location
                0,                                  // binding
                vkpp::Format::eRGBA32sFloat,        // format
                offsetof(VertexData, inPosition)    // offset
            },
            // Location 1: Color
            {
                1,                                  // location
                0,                                  // binding
                vkpp::Format::eRGBA32sFloat,        // format
                offsetof(VertexData, inColor)       // offset
            },
            // Location 2: Texture Coordinates
            {
                2,                                  // location
                0,                                  // binding
                vkpp::Format::eRG32sFloat,          // format
                offsetof(VertexData, texCoord)      // offset
            },
            // Location 3: Normal
            {
                3,                                  // location
                0,                                  // binding
                vkpp::Format::eRGB32sFloat,         // format
                offsetof(VertexData, normal)        // offset
            }
        } };

        return lInputAttributeDescriptions;
    }
};



struct Model
{
    constexpr static auto DefaultImporterFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;

    /*BufferResource vertices;
    BufferResource indices;*/

    // Stores vertex and index bases and counts for each part of a model.
    struct ModelPart
    {
        uint32_t vertexBase{ 0 };
        uint32_t vertexCount{ 0 };
        uint32_t indexBase{ 0 };
        uint32_t indexCount{ 0 };

        ModelPart(uint32_t aVertexBase, uint32_t aVertexCount, uint32_t aIndexBase, uint32_t aIndexCount)
            : vertexBase(aVertexBase), vertexCount(aVertexCount), indexBase(aIndexBase), indexCount(aIndexCount)
        {}
    };

    std::vector<ModelPart> modelParts;

    explicit Model(const std::string& aFilename, unsigned int aImporterFlags = DefaultImporterFlags)
    {
        Assimp::Importer lImporter;

        const auto lpAIScene = lImporter.ReadFile(aFilename, aImporterFlags);
        assert(lpAIScene);

        // modelParts.resize(lpAIScene->mNumMeshes);

        glm::vec3 lScale{ 1.0f }, /*lUVScale{ 1.0f },*/ lCenter{ 0.0f };

        std::vector<float> lVertexBuffer;
        std::vector<uint32_t> lIndexBuffer;

        uint32_t lVertexCount{ 0 }, lIndexCount{ 0 };

        for (unsigned int lIndex = 0; lIndex < lpAIScene->mNumMeshes; ++lIndex)
        {
            const auto lpAIMesh = lpAIScene->mMeshes[lIndex];

            modelParts.emplace_back(lVertexCount, lpAIMesh->mNumVertices, lIndexCount, lpAIMesh->mNumFaces * 3);
            lVertexCount += lpAIScene->mMeshes[lIndex]->mNumVertices;
            lIndexCount += lpAIScene->mMeshes[lIndex]->mNumFaces * 3;

            aiColor3D lColor;
            lpAIScene->mMaterials[lpAIMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, lColor);

            // const aiVector3D lZero3D;

            for (unsigned int lVerIndex = 0; lVerIndex < lpAIMesh->mNumVertices; ++lVerIndex)
            {
                // Vertex positions.
                const auto lPos = lpAIMesh->mVertices[lVerIndex];
                lVertexBuffer.emplace_back(lPos.x * lScale.x + lCenter.x);
                lVertexBuffer.emplace_back(-lPos.y * lScale.y + lCenter.y);
                lVertexBuffer.emplace_back(lPos.z * lScale.z + lCenter.z);

                // Vertex normals.
                const auto lNormal = lpAIMesh->mNormals[lVerIndex];
                lVertexBuffer.emplace_back(lNormal.x);
                lVertexBuffer.emplace_back(-lNormal.y);
                lVertexBuffer.emplace_back(lNormal.z);
            }

            uint32_t lIndexBase{ 0 };
            for (unsigned int lIdxIndex = 0; lIdxIndex < lpAIMesh->mNumFaces; ++lIdxIndex)
            {
                const auto& lFace = lpAIMesh->mFaces[lIdxIndex];

                if (lFace.mNumIndices != 3)
                    continue;

                lIndexBuffer.emplace_back(lIndexBase + lFace.mIndices[0]);
                lIndexBuffer.emplace_back(lIndexBase + lFace.mIndices[1]);
                lIndexBuffer.emplace_back(lIndexBase + lFace.mIndices[2]);
            }
        }
    }
};



class ObjModel
{
private:
    tinyobj::attrib_t mAttrib;
    std::vector<tinyobj::shape_t> mShapes;
    std::vector<tinyobj::material_t> mMaterials;

public:
    explicit ObjModel(const char* aFilename)
    {
        std::string lErrMsg;
        auto lRet = tinyobj::LoadObj(&mAttrib, &mShapes, &mMaterials, &lErrMsg, aFilename);
        assert(lRet && lErrMsg.empty());
    }
};



class MultiPipelines : public ExampleBase, public CWindowEvent
{
private:
    vkpp::CommandPool mCmdPool;
    std::vector<vkpp::CommandBuffer> mCmdDrawBuffers;

    vkpp::RenderPass mRenderPass;
    ImageResource mDepthResource;
    std::vector<vkpp::Framebuffer> mFramebuffers;
    vkpp::DescriptorSetLayout mSetLayout;
    vkpp::PipelineLayout mPipelineLayout;

    struct
    {
        vkpp::Pipeline phong;
        vkpp::Pipeline wireframe;
        vkpp::Pipeline toon;
    } mPipelines;

    vkpp::DescriptorPool mDescriptorPool;
    vkpp::DescriptorSet mDescriptorSet;

    void CreateCommandPool(void);
    void AllocateDrawCmdBuffers(void);

    void CreateRenderPass(void);
    void CreateDepthResource(void);
    void CreateFramebuffer(void);
    void CreateUniformBuffer(void);
    void CreateSetLayout(void);
    void CreatePipelineLayout(void);

    void CreateGraphicsPipelines(void);

    void CreateDescriptorPool(void);
    void AllocateDescriptorSet(void);

public:
    MultiPipelines(CWindow& aWindow, const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~MultiPipelines(void);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_MULTIPIPELINE_H__
