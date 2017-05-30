#ifndef __VKPP_TYPE_DESCRIPTOR_POOL_H__
#define __VKPP_TYPE_DESCRIPTOR_POOL_H__



#include <Info/Common.h>

#include <Type/DescriptorSet.h>



namespace vkpp
{



enum class DescriptorPoolResetFlagBits
{};

VKPP_ENUM_BIT_MASK_FLAGS(DescriptorPoolReset)



struct DescriptorPoolSize : public internal::VkTrait<DescriptorPoolSize, VkDescriptorPoolSize>
{
    DescriptorType  type;
    uint32_t        descriptorCount{ 0 };

    DEFINE_CLASS_MEMBER(DescriptorPoolSize)

    constexpr DescriptorPoolSize(DescriptorType aType, uint32_t aDescriptorCount) noexcept : type(aType), descriptorCount(aDescriptorCount)
    {}
};

ConsistencyCheck(DescriptorPoolSize, type, descriptorCount)



enum class DescriptorPoolCreateFlagBits
{
    eFreeDescriptorSet  = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
};

VKPP_ENUM_BIT_MASK_FLAGS(DescriptorPoolCreate)



class DescriptorPoolCreateInfo : public internal::VkTrait<DescriptorPoolCreateInfo, VkDescriptorPoolCreateInfo>
{
private:
    const internal::Structure   sType = internal::Structure::eDescriptorPool;

    constexpr DescriptorPoolCreateInfo(DescriptorPoolSize&& aPoolSize, uint32_t aMaxSets, const DescriptorPoolCreateFlags& aFlags = DefaultFlags) noexcept = delete;

public:
    const void*                 pNext{ nullptr };
    DescriptorPoolCreateFlags   flags;
    uint32_t                    maxSets{ 0 };
    uint32_t                    poolSizeCount{ 0 };
    const DescriptorPoolSize*   pPoolSizes{ nullptr };

    DEFINE_CLASS_MEMBER(DescriptorPoolCreateInfo)

    constexpr DescriptorPoolCreateInfo(uint32_t aPoolSizeCount, const DescriptorPoolSize* apPoolSizes, uint32_t aMaxSets, const DescriptorPoolCreateFlags& aFlags = DefaultFlags) noexcept
        : flags(aFlags), maxSets(aMaxSets), poolSizeCount(aPoolSizeCount), pPoolSizes(apPoolSizes)
    {}

    constexpr DescriptorPoolCreateInfo(const DescriptorPoolSize& aPoolSize, uint32_t aMaxSets, const DescriptorPoolCreateFlags& aFlags = DefaultFlags) noexcept
        : DescriptorPoolCreateInfo(1, aPoolSize.AddressOf(), aMaxSets, aFlags)
    {}

    template <typename D, typename = EnableIfValueType<ValueType<D>, DescriptorPoolSize>>
    constexpr DescriptorPoolCreateInfo(D&& aPoolSizes, uint32_t aMaxSets, const DescriptorPoolCreateFlags& aFlags = DefaultFlags) noexcept
        : DescriptorPoolCreateInfo(static_cast<uint32_t>(aPoolSizes.size()), aPoolSizes.data(), aMaxSets, aFlags)
    {
        StaticLValueRefAssert(D, aPoolSizes);
    }

    DescriptorPoolCreateInfo& SetFlags(const DescriptorPoolCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    DescriptorPoolCreateInfo& SetMaxSets(uint32_t aMaxSets)
    {
        maxSets = aMaxSets;

        return *this;
    }

    DescriptorPoolCreateInfo& SetPoolSizes(uint32_t aPoolSizeCount, const DescriptorPoolSize* apPoolSizes)
    {
        poolSizeCount   = aPoolSizeCount;
        pPoolSizes      = apPoolSizes;

        return *this;
    }

    template <typename D, typename = EnableIfValueType<ValueType<D>, DescriptorPoolSize>>
    DescriptorPoolCreateInfo& SetPoolSizes(D&& aPoolSizes)
    {
        StaticLValueRefAssert(D, aPoolSizes);

        return SetPoolSizes(static_cast<uint32_t>(aPoolSizes.size()), aPoolSizes.data());
    }
};

ConsistencyCheck(DescriptorPoolCreateInfo, pNext, flags, maxSets, poolSizeCount, pPoolSizes)



class DescriptorPool : public internal::VkTrait<DescriptorPool, VkDescriptorPool>
{
private:
    VkDescriptorPool mDescriptorPool{ VK_NULL_HANDLE };

public:
    DescriptorPool(void) = default;

    DescriptorPool(std::nullptr_t)
    {}

    explicit DescriptorPool(VkDescriptorPool aDescriptorPool) : mDescriptorPool(aDescriptorPool)
    {}
};

StaticSizeCheck(DescriptorPool)



class DescriptorSetAllocateInfo : public internal::VkTrait<DescriptorSetAllocateInfo, VkDescriptorSetAllocateInfo>
{
private:
    const internal::Structure   sType = internal::Structure::eDescriptorSetAllocate;

    DescriptorSetAllocateInfo(const DescriptorPool& aDescriptorPool, DescriptorSetLayout&& aSetLayout) noexcept = delete;

public:
    const void*                 pNext{ nullptr };
    DescriptorPool              descriptorPool;
    uint32_t                    descriptorSetCount{ 0 };
    const DescriptorSetLayout*  pSetLayouts{ nullptr };

    DEFINE_CLASS_MEMBER(DescriptorSetAllocateInfo)

    DescriptorSetAllocateInfo(const DescriptorPool& aDescriptorPool, uint32_t aDescriptorSetCount, const DescriptorSetLayout* apSetLayouts) noexcept
        : descriptorPool(aDescriptorPool), descriptorSetCount(aDescriptorSetCount), pSetLayouts(apSetLayouts)
    {}

    DescriptorSetAllocateInfo(const DescriptorPool& aDescriptorPool, const DescriptorSetLayout& aSetLayout) noexcept
        : DescriptorSetAllocateInfo(aDescriptorPool, 1, aSetLayout.AddressOf())
    {}

    template <typename D, typename = EnableIfValueType<ValueType<D>, DescriptorSetLayout>>
    DescriptorSetAllocateInfo(const DescriptorPool& aDescriptorPool, D&& aSetLayouts) noexcept
        : DescriptorSetAllocateInfo(aDescriptorPool, static_cast<uint32_t>(aSetLayouts.size()), aSetLayouts.data())
    {
        StaticLValueRefAssert(D, aSetLayouts);
    }

    DescriptorSetAllocateInfo& SetNext(const void* apNext)
    {
        pNext   = apNext;

        return *this;
    }

    DescriptorSetAllocateInfo& SetDescriptorPool(const DescriptorPool& aDescriptorPool)
    {
        descriptorPool  = aDescriptorPool;

        return *this;
    }

    DescriptorSetAllocateInfo& SetDescriptorSetLayouts(uint32_t aDescriptorSetCount, const DescriptorSetLayout* apSetLayouts)
    {
        descriptorSetCount  = aDescriptorSetCount;
        pSetLayouts         = apSetLayouts;

        return *this;
    }

    template <typename D, typename = EnableIfValueType<ValueType<D>, DescriptorSetLayout>>
    DescriptorSetAllocateInfo& SetDescriptorSetLayouts(D&& aSetLayouts)
    {
        StaticLValueRefAssert(D, aSetLayouts);

        return SetDescriptorSetLayouts(static_cast<uint32_t>(aSetLayouts.size()), aSetLayouts.data());
    }
};

ConsistencyCheck(DescriptorSetAllocateInfo, pNext, descriptorPool, descriptorSetCount, pSetLayouts)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_DESCRIPTOR_POOL_H__
