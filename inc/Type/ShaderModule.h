#ifndef __VKPP_TYPE_SHADER_MODULE_H__
#define __VKPP_TYPE_SHADER_MODULE_H__



#include <Info/Common.h>
#include <Info/Flags.h>



namespace vkpp
{



enum class ShaderStageFlagBits
{
    eVertex                 = VK_SHADER_STAGE_VERTEX_BIT,
    eTessellationControl    = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    eTessellationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
    eGeometry               = VK_SHADER_STAGE_GEOMETRY_BIT,
    eFragment               = VK_SHADER_STAGE_FRAGMENT_BIT,
    eCompute                = VK_SHADER_STAGE_COMPUTE_BIT,
    eAllGraphics            = VK_SHADER_STAGE_ALL_GRAPHICS,
    eAll                    = VK_SHADER_STAGE_ALL
};

using ShaderStageFlags = internal::Flags<ShaderStageFlagBits, VkShaderStageFlags>;



enum class ShaderModuleCreateFlagBits
{};

using ShaderModuleCreateFlags = internal::Flags<ShaderModuleCreateFlagBits, VkShaderModuleCreateFlags>;



class ShaderModuleCreateInfo : public internal::VkTrait<ShaderModuleCreateInfo, VkShaderModuleCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eShaderModule;

public:
    const void*             pNext{ nullptr };
    ShaderModuleCreateFlags flags;
    std::size_t             codeSize{ 0 };
    const uint32_t*         pCode{ nullptr };

    DEFINE_CLASS_MEMBER(ShaderModuleCreateInfo)

    ShaderModuleCreateInfo(const std::size_t aCodeSize, const uint32_t* apCode, const ShaderModuleCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), codeSize(aCodeSize), pCode(apCode)
    {}

    explicit ShaderModuleCreateInfo(const std::vector<uint32_t>& aCode, const ShaderModuleCreateFlags& aFlags = DefaultFlags)
        : ShaderModuleCreateInfo(aCode.size(), aCode.data(), aFlags)
    {}

    ShaderModuleCreateInfo& SetFlags(const ShaderModuleCreateFlags& aFlags)
    {
        flags   = aFlags;

        return *this;
    }

    ShaderModuleCreateInfo& SetCode(std::size_t aCodeSize, const uint32_t* apCode)
    {
        codeSize    = aCodeSize;
        pCode       = apCode;

        return *this;
    }

    ShaderModuleCreateInfo& SetCode(const std::vector<uint32_t>& aCode)
    {
        return SetCode(aCode.size(), aCode.data());
    }
};

StaticSizeCheck(ShaderModuleCreateInfo);



class ShaderModule : public internal::VkTrait<ShaderModule, VkShaderModule>
{
private:
    VkShaderModule mShaderModule{ VK_NULL_HANDLE };

public:
    ShaderModule(void) = default;

    ShaderModule(std::nullptr_t)
    {}

    explicit ShaderModule(VkShaderModule aShaderModule) : mShaderModule(aShaderModule)
    {}
};

StaticSizeCheck(ShaderModule);


}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_SHADER_MODULE_H__
