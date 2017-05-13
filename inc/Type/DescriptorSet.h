#ifndef __VKPP_TYPE_DESCRIPTOR_SET_H__
#define __VKPP_TYPE_DESCRIPTOR_SET_H__



#include <Info/Common.h>
#include <Info/Flags.h>

#include <Type/ShaderModule.h>
#include <Type/Sampler.h>
#include <Type/Image.h>
#include <Type/Buffer.h>



namespace vkpp
{



enum class DescriptorType
{
    eSampler                = VK_DESCRIPTOR_TYPE_SAMPLER,
    eCombinedImageSampler   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    eSampledImage           = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
    eStorageImage           = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    eUniformTexelBuffer     = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
    eStorageTexelBuffer     = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
    eUniformBuffer          = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    eStorageBuffer          = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    eUniformBufferDynamic   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
    eStorageBufferDynamic   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
    eInputAttachment        = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
};



struct DescriptorSetLayoutBinding : public internal::VkTrait<DescriptorSetLayoutBinding, VkDescriptorSetLayoutBinding>
{
    uint32_t            binding{ 0 };
    DescriptorType      descriptorType{ DescriptorType::eSampler };
    uint32_t            descriptorCount{ 0 };
    ShaderStageFlags    stageFlags;
    const Sampler*      pImmutableSamplers{ nullptr };

    DEFINE_CLASS_MEMBER(DescriptorSetLayoutBinding)

    constexpr DescriptorSetLayoutBinding(uint32_t aBinding, DescriptorType aDescriptorType, uint32_t aDescriptorCount, const ShaderStageFlags& aStageFlags, const Sampler* apImmutableSamplers = nullptr) noexcept
        : binding(aBinding), descriptorType(aDescriptorType), descriptorCount(aDescriptorCount), stageFlags(aStageFlags), pImmutableSamplers(apImmutableSamplers)
    {}

    DescriptorSetLayoutBinding& SetBinding(uint32_t aBinding)
    {
        binding = aBinding;

        return *this;
    }

    DescriptorSetLayoutBinding& SetDescriptorType(DescriptorType aDescriptorType, uint32_t aDescriptorCount)
    {
        descriptorType  = aDescriptorType;
        descriptorCount = aDescriptorCount;

        return *this;
    }

    DescriptorSetLayoutBinding& SetShaderStageFlags(const ShaderStageFlags& aFlags)
    {
        stageFlags  = aFlags;

        return *this;
    }

    DescriptorSetLayoutBinding& SetImmutableSamplers(const Sampler* apImmultableSamplers)
    {
        pImmutableSamplers = apImmultableSamplers;

        return *this;
    }
};

ConsistencyCheck(DescriptorSetLayoutBinding, binding, descriptorType, descriptorCount, stageFlags, pImmutableSamplers)



enum class DescriptorSetLayoutCreateFlagBits
{
    ePushDescriptorKHR      = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR
};

VKPP_ENUM_BIT_MASK_FLAGS(DescriptorSetLayoutCreate)



class DescriptorSetLayoutCreateInfo : public internal::VkTrait<DescriptorSetLayoutCreateInfo, VkDescriptorSetLayoutCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eDescriptorSetLayout;

public:
    const void*                         pNext{ nullptr };
    DescriptorSetLayoutCreateFlags      flags;
    uint32_t                            bindingCount{ 0 };
    const DescriptorSetLayoutBinding*   pBindings{ nullptr };

    DEFINE_CLASS_MEMBER(DescriptorSetLayoutCreateInfo)

    constexpr DescriptorSetLayoutCreateInfo(uint32_t aBindingCount, const DescriptorSetLayoutBinding* apBindings, const DescriptorSetLayoutCreateFlags& aFlags = DefaultFlags) noexcept
        : flags(aFlags), bindingCount(aBindingCount), pBindings(apBindings)
    {}

    template <typename B, typename = EnableIfValueType<ValueType<B>, DescriptorSetLayoutBinding>>
    explicit constexpr DescriptorSetLayoutCreateInfo(B&& aBindings, const DescriptorSetLayoutCreateFlags& aFlags = DefaultFlags) noexcept
        : DescriptorSetLayoutCreateInfo(static_cast<uint32_t>(aBindings.size()), aBindings.data(), aFlags)
    {
        StaticLValueRefAssert(B, aBindings);
    }

    DescriptorSetLayoutCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    DescriptorSetLayoutCreateInfo& SetFlags(const DescriptorSetLayoutCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    DescriptorSetLayoutCreateInfo& SetBindings(uint32_t aBindingCount, const DescriptorSetLayoutBinding* apBindings)
    {
        bindingCount    = aBindingCount;
        pBindings       = apBindings;

        return *this;
    }

    template <typename B, typename = EnableIfValueType<ValueType<B>, DescriptorSetLayoutBinding>>
    DescriptorSetLayoutCreateInfo& SetBindings(B&& aBindings)
    {
        StaticLValueRefAssert(B, aBindings);

        return SetBindings(static_cast<uint32_t>(aBindings.size()), aBindings.data());
    }
};

ConsistencyCheck(DescriptorSetLayoutCreateInfo, pNext, flags, bindingCount, pBindings)



class DescriptorSetLayout : public internal::VkTrait<DescriptorSetLayout, VkDescriptorSetLayout>
{
private:
    VkDescriptorSetLayout mDescriptorSetLayout{ VK_NULL_HANDLE };

public:
    DescriptorSetLayout(void) = default;

    DescriptorSetLayout(std::nullptr_t)
    {}

    explicit DescriptorSetLayout(VkDescriptorSetLayout aDescriptorSetLayout) : mDescriptorSetLayout(aDescriptorSetLayout)
    {}
};

StaticSizeCheck(DescriptorSetLayout)



class DescriptorSet : public internal::VkTrait<DescriptorSet, VkDescriptorSet>
{
private:
    VkDescriptorSet mDescriptorSet{ VK_NULL_HANDLE };

public:
    DescriptorSet(void) = default;

    DescriptorSet(std::nullptr_t)
    {}

    explicit DescriptorSet(VkDescriptorSet aDescriptorSet) : mDescriptorSet(aDescriptorSet)
    {}
};

StaticSizeCheck(DescriptorSet)



struct DescriptorImageInfo : public internal::VkTrait<DescriptorImageInfo, VkDescriptorImageInfo>
{
    Sampler     sampler;
    ImageView   imageView;
    ImageLayout imageLayout;

    DEFINE_CLASS_MEMBER(DescriptorImageInfo)

    DescriptorImageInfo(const Sampler& aSampler, const ImageView& aImageView, const ImageLayout& aImageLayout)
        : sampler(aSampler), imageView(aImageView), imageLayout(aImageLayout)
    {}

    DescriptorImageInfo& SetSampler(const Sampler& aSampler)
    {
        sampler = aSampler;

        return *this;
    }

    DescriptorImageInfo& SetImageView(const ImageView& aImageView)
    {
        imageView = aImageView;

        return *this;
    }

    DescriptorImageInfo& SetImageLayout(const ImageLayout& aImageLayout)
    {
        imageLayout = aImageLayout;

        return *this;   
    }
};

ConsistencyCheck(DescriptorImageInfo, sampler, imageView, imageLayout)



struct DescriptorBufferInfo : public internal::VkTrait<DescriptorBufferInfo, VkDescriptorBufferInfo>
{
    Buffer      buffer;
    DeviceSize  offset{ 0 };
    DeviceSize  range{ 0 };

    DEFINE_CLASS_MEMBER(DescriptorBufferInfo)

    constexpr DescriptorBufferInfo(const Buffer& aBuffer, DeviceSize aOffset, DeviceSize aRange) noexcept
        : buffer(aBuffer), offset(aOffset), range(aRange)
    {}

    DescriptorBufferInfo& SetBuffer(const Buffer& aBuffer)
    {
        buffer = aBuffer;

        return *this;
    }

    DescriptorBufferInfo& SetExtent(DeviceSize aOffset, DeviceSize aRange)
    {
        offset  = aOffset;
        range   = aRange;

        return *this;
    }
};

ConsistencyCheck(DescriptorBufferInfo, buffer, offset, range)



class WriteDescriptorSetInfo : public internal::VkTrait<WriteDescriptorSetInfo, VkWriteDescriptorSet>
{
private:
    const internal::Structure sType = internal::Structure::eWriteDescriptorSet;

public:
    const void*                 pNext{ nullptr };
    DescriptorSet               dstSet;
    uint32_t                    dstBinding{ 0 };
    uint32_t                    dstArrayElement{ 0 };
    uint32_t                    descriptorCount{ 0 };
    DescriptorType              descriptorType;
    const DescriptorImageInfo*  pImageInfo{ nullptr };
    const DescriptorBufferInfo* pBufferInfo{ nullptr };
    const BufferView*           pTexelBufferView{ nullptr };

    DEFINE_CLASS_MEMBER(WriteDescriptorSetInfo)

    constexpr WriteDescriptorSetInfo(const DescriptorSet& aDstSet, uint32_t aDstBinding, uint32_t aDstArrayElement, uint32_t aDescriptorCount, DescriptorType aDescriptorType,
        const DescriptorImageInfo* apImageInfo = nullptr, const DescriptorBufferInfo* apBufferInfo = nullptr, const BufferView* apTexelBufferView = nullptr) noexcept
        : dstSet(aDstSet), dstBinding(aDstBinding), dstArrayElement(aDstArrayElement), descriptorCount(aDescriptorCount), descriptorType(aDescriptorType),
          pImageInfo(apImageInfo), pBufferInfo(apBufferInfo), pTexelBufferView(apTexelBufferView)
    {}

    constexpr WriteDescriptorSetInfo(const DescriptorSet& aDstSet, uint32_t aDstBinding, uint32_t aDstArrayElement, DescriptorType aDescriptorType,
        const DescriptorImageInfo& aImageInfo) noexcept
        : WriteDescriptorSetInfo(aDstSet, aDstBinding, aDstArrayElement, 1, aDescriptorType, aImageInfo.AddressOf())
    {}

    constexpr WriteDescriptorSetInfo(const DescriptorSet& aDstSet, uint32_t aDstBinding, uint32_t aDstArrayElement, DescriptorType aDescriptorType,
        const DescriptorBufferInfo& aBufferInfo) noexcept
        : WriteDescriptorSetInfo(aDstSet, aDstBinding, aDstArrayElement, 1, aDescriptorType, nullptr, aBufferInfo.AddressOf())
    {}

    constexpr WriteDescriptorSetInfo(const DescriptorSet& aDstSet, uint32_t aDstBinding, uint32_t aDstArrayElement, DescriptorType aDescriptorType,
        const BufferView& aTexelBufferView) noexcept
        : WriteDescriptorSetInfo(aDstSet, aDstBinding, aDstArrayElement, 1, aDescriptorType, nullptr, nullptr, aTexelBufferView.AddressOf())
    {}

    WriteDescriptorSetInfo& SetDstDescriptorSet(const DescriptorSet& aDstSet)
    {
        dstSet = aDstSet;

        return *this;
    }

    WriteDescriptorSetInfo& SetBinding(uint32_t aBinding, uint32_t aDstArrayElement, uint32_t aDescriptorCount)
    {
        dstBinding      = aBinding;
        dstArrayElement = aDstArrayElement;
        descriptorCount = aDescriptorCount;

        return *this;
    }

    WriteDescriptorSetInfo& SetDescriptorType(DescriptorType aDescriptorType)
    {
        descriptorType = aDescriptorType;

        return *this;
    }

    // TODO: apImageInfo points to an array.
    WriteDescriptorSetInfo& SetImageInfo(const DescriptorImageInfo* apImageInfo)
    {
        pImageInfo = apImageInfo;

        return *this;
    }

    WriteDescriptorSetInfo& SetImageInfo(const DescriptorImageInfo& aImageInfo)
    {
        return SetImageInfo(aImageInfo.AddressOf());
    }

    WriteDescriptorSetInfo& SetBufferInfo(const DescriptorBufferInfo* apBufferInfo)
    {
        pBufferInfo = apBufferInfo;

        return *this;
    }

    WriteDescriptorSetInfo& SetBufferInfo(const DescriptorBufferInfo& aBufferInfo)
    {
        return SetBufferInfo(aBufferInfo.AddressOf());
    }

    WriteDescriptorSetInfo& SetTexelBufferView(const BufferView* apTexelBufferView)
    {
        pTexelBufferView = apTexelBufferView;

        return *this;
    }

    WriteDescriptorSetInfo& SetTexelBufferView(const BufferView& aTexelBufferView)
    {
        return SetTexelBufferView(aTexelBufferView.AddressOf());
    }
};

ConsistencyCheck(WriteDescriptorSetInfo, pNext, dstSet, dstBinding, dstArrayElement, descriptorCount, descriptorType, pImageInfo, pBufferInfo, pTexelBufferView)



class CopyDescriptorSetInfo : public internal::VkTrait<CopyDescriptorSetInfo, VkCopyDescriptorSet>
{
private:
    const internal::Structure sType = internal::Structure::eCopyDescriptorSet;

public:
    const void*         pNext{ nullptr };
    DescriptorSet       srcSet;
    uint32_t            srcBinding{ 0 };
    uint32_t            srcArrayElement{ 0 };
    DescriptorSet       dstSet;
    uint32_t            dstBinding{ 0 };
    uint32_t            dstArrayElement{ 0 };
    uint32_t            descriptorCount{ 0 };

    DEFINE_CLASS_MEMBER(CopyDescriptorSetInfo)

    CopyDescriptorSetInfo(const DescriptorSet& aSrcSet, uint32_t aSrcBinding, uint32_t aSrcArrayElement,
        const DescriptorSet& aDstSet, uint32_t aDstBinding, uint32_t aDstArrayElement, uint32_t aDescriptorCount)
        : srcSet(aSrcSet), srcBinding(aSrcBinding), srcArrayElement(aSrcArrayElement),
          dstSet(aDstSet), dstBinding(aDstBinding), dstArrayElement(aDstArrayElement), descriptorCount(aDescriptorCount)
    {}

    CopyDescriptorSetInfo& SetSrcDescriptorSet(const DescriptorSet& aSrcSet, uint32_t aSrcBinding, uint32_t aSrcArrayElement)
    {
        srcSet          = aSrcSet;
        srcBinding      = aSrcBinding;
        srcArrayElement = aSrcArrayElement;

        return *this;
    }

    CopyDescriptorSetInfo& SetDstDescriptorSet(const DescriptorSet& aDstSet, uint32_t aDstBinding, uint32_t aDstArrayElement)
    {
        dstSet          = aDstSet;
        dstBinding      = aDstBinding;
        dstArrayElement = aDstArrayElement;

        return *this;
    }

    CopyDescriptorSetInfo& SetDescriptorCount(uint32_t aDescriptorCount)
    {
        descriptorCount = aDescriptorCount;

        return *this;
    }
};

ConsistencyCheck(CopyDescriptorSetInfo, pNext, srcSet, srcBinding, srcArrayElement, dstSet, dstBinding, dstArrayElement, descriptorCount)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_DESCRIPTOR_SET_H__
