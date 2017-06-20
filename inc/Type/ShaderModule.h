#ifndef __VKPP_TYPE_SHADER_MODULE_H__
#define __VKPP_TYPE_SHADER_MODULE_H__



#include <Info/Common.h>
#include <Info/Flags.h>



namespace vkpp
{



struct SpecializationMapEntry : public internal::VkTrait<SpecializationMapEntry, VkSpecializationMapEntry>
{
    uint32_t        constantID{ 0 };
    uint32_t        offset{ 0 };
    std::size_t     size{ 0 };

    DEFINE_CLASS_MEMBER(SpecializationMapEntry)

    constexpr SpecializationMapEntry(uint32_t aConstantID, uint32_t aOffset, std::size_t aSize) noexcept
        : constantID(aConstantID), offset(aOffset), size(aSize)
    {}

    SpecializationMapEntry& SetConstantID(uint32_t aConstantID) noexcept
    {
        constantID = aConstantID;

        return *this;
    }

    SpecializationMapEntry& SetSize(uint32_t aOffset, std::size_t aSize) noexcept
    {
        offset  = aOffset;
        size    = aSize;

        return *this;
    }
};

ConsistencyCheck(SpecializationMapEntry, constantID, offset, size)



class SpecializationInfo : public internal::VkTrait<SpecializationInfo, VkSpecializationInfo>
{
private:
    constexpr SpecializationInfo(SpecializationMapEntry&&, std::size_t, const void*) noexcept = delete;

    SpecializationInfo& SetMapEntry(SpecializationMapEntry&&) noexcept = delete;

public:
    uint32_t                        mapEntryCount{ 0 };
    const SpecializationMapEntry*   pMapEntries{ nullptr };
    std::size_t                     dataSize{ 0 };
    const void*                     pData{ nullptr };

    DEFINE_CLASS_MEMBER(SpecializationInfo)

    constexpr SpecializationInfo(uint32_t aMapEntryCount, const SpecializationMapEntry* apMapEntries, std::size_t aDataSize, const void* apData) noexcept
        : mapEntryCount(aMapEntryCount), pMapEntries(apMapEntries), dataSize(aDataSize), pData(apData)
    {
        assert(aMapEntryCount != 0 && apMapEntries != nullptr);
        assert(aDataSize != 0 && apData != nullptr);
    }

    constexpr SpecializationInfo(const SpecializationMapEntry& aMapEntry, std::size_t aDataSize, const void* apData) noexcept
        : SpecializationInfo(1, aMapEntry.AddressOf(), aDataSize, apData)
    {}

    template <typename D>
    constexpr SpecializationInfo(const SpecializationMapEntry& aMapEntry, D&& aData) noexcept
        : SpecializationInfo(1, aMapEntry.AddressOf(), sizeof(D), &aData)
    {
        StaticLValueRefAssert(D, aData);
    }

    template <typename M, typename = EnableIfValueType<ValueType<M>, SpecializationMapEntry>>
    constexpr SpecializationInfo(M&& aMapEntries, std::size_t aDataSize, const void* apData) noexcept
        : SpecializationInfo(SizeOf<uint32_t>(aMapEntries), DataOf(aMapEntries), aDataSize, apData)
    {
        StaticLValueRefAssert(M, aMapEntries);
    }

    template <typename M, typename D, typename = EnableIfValueType<ValueType<M>, SpecializationMapEntry>>
    constexpr SpecializationInfo(M&& aMapEntries, D&& aData) noexcept
        : SpecializationInfo(SizeOf<uint32_t>(aMapEntries), DataOf(aMapEntries), sizeof(D), &aData)
    {
        StaticLValueRefAssert(M, aMapEntries);
        StaticLValueRefAssert(D, aData);
    }

    SpecializationInfo& SetMapEntries(uint32_t aMapEntryCount, const SpecializationMapEntry* apMapEntries) noexcept
    {
        assert(aMapEntryCount != 0 && apMapEntries != nullptr);

        mapEntryCount   = aMapEntryCount;
        pMapEntries     = apMapEntries;

        return *this;
    }

    SpecializationInfo& SetMapEntry(const SpecializationMapEntry& aMapEntry) noexcept
    {
        mapEntryCount   = 1;
        pMapEntries     = aMapEntry.AddressOf();

        return *this;
    }

    template <typename M, typename = EnableIfValueType<ValueType<M>, SpecializationMapEntry>>
    SpecializationInfo& SetMapEntries(M&& aMapEntries) noexcept
    {
        StaticLValueRefAssert(M, aMapEntries);

        return SetMapEntries(SizeOf<uint32_t>(aMapEntries), DataOf(aMapEntries));
    }

    SpecializationInfo& SetData(const std::size_t aDataSize, const void* apData) noexcept
    {
        assert(aDataSize != 0 && apData != nullptr);

        dataSize    = aDataSize;
        pData       = apData;

        return *this;
    }

    template <typename D>
    SpecializationInfo& SetData(D&& aData) noexcept
    {
        StaticLValueRefAssert(D, aData);

        return SetData(sizeof(D), &aData);
    }
};

ConsistencyCheck(SpecializationInfo, mapEntryCount, pMapEntries, dataSize, pData)



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

VKPP_ENUM_BIT_MASK_FLAGS(ShaderStage)



enum class ShaderModuleCreateFlagBits
{};

VKPP_ENUM_BIT_MASK_FLAGS(ShaderModuleCreate)



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

    constexpr ShaderModuleCreateInfo(const std::size_t aCodeSize, const uint32_t* apCode, const ShaderModuleCreateFlags& aFlags = DefaultFlags) noexcept
        : flags(aFlags), codeSize(aCodeSize), pCode(apCode)
    {}

    ShaderModuleCreateInfo(const std::vector<uint32_t>& aCode, const ShaderModuleCreateFlags& aFlags = DefaultFlags) noexcept
        : ShaderModuleCreateInfo(aCode.size(), aCode.data(), aFlags)
    {}

    ShaderModuleCreateInfo(const std::vector<char>& aCode, const ShaderModuleCreateFlags& aFlags = DefaultFlags) noexcept
        : ShaderModuleCreateInfo(aCode.size(), reinterpret_cast<const uint32_t*>(aCode.data()), aFlags)
    {}

    ShaderModuleCreateInfo& SetFlags(const ShaderModuleCreateFlags& aFlags) noexcept
    {
        flags   = aFlags;

        return *this;
    }

    ShaderModuleCreateInfo& SetCode(std::size_t aCodeSize, const uint32_t* apCode) noexcept
    {
        codeSize    = aCodeSize;
        pCode       = apCode;

        return *this;
    }

    ShaderModuleCreateInfo& SetCode(const std::vector<uint32_t>& aCode) noexcept
    {
        return SetCode(aCode.size(), aCode.data());
    }
};

ConsistencyCheck(ShaderModuleCreateInfo, pNext, flags, codeSize, pCode)



class ShaderModule : public internal::VkTrait<ShaderModule, VkShaderModule>
{
private:
    VkShaderModule mShaderModule{ VK_NULL_HANDLE };

public:
    ShaderModule(void) noexcept = default;

    ShaderModule(std::nullptr_t) noexcept
    {}

    explicit ShaderModule(VkShaderModule aShaderModule) noexcept : mShaderModule(aShaderModule)
    {}
};

StaticSizeCheck(ShaderModule)


}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_SHADER_MODULE_H__
