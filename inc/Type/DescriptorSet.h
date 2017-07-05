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
    uint32_t            binding{ 0 };                                   // binding is the binding number of this entry and corresponds to a resource of the same binding number in the shader stages.
    DescriptorType      descriptorType{ DescriptorType::eSampler };
    uint32_t            descriptorCount{ 0 };                           // descriptorCount is the number of descriptors contained in the binding, accessed in a shader as an array.
    ShaderStageFlags    stageFlags;
    const Sampler*      pImmutableSamplers{ nullptr };

    DEFINE_CLASS_MEMBER(DescriptorSetLayoutBinding)

    constexpr DescriptorSetLayoutBinding(uint32_t aBinding, DescriptorType aDescriptorType, uint32_t aDescriptorCount, const ShaderStageFlags& aStageFlags, const Sampler* apImmutableSamplers = nullptr) noexcept
        : binding(aBinding), descriptorType(aDescriptorType), descriptorCount(aDescriptorCount), stageFlags(aStageFlags), pImmutableSamplers(apImmutableSamplers)
    {}

    constexpr DescriptorSetLayoutBinding(uint32_t aBinding, DescriptorType aDescriptorType, const ShaderStageFlags& aStageFlags, const Sampler* apImmutableSamplers = nullptr) noexcept
        : DescriptorSetLayoutBinding(aBinding, aDescriptorType, 1, aStageFlags, apImmutableSamplers)
    {}

    DescriptorSetLayoutBinding& SetBinding(uint32_t aBinding) noexcept
    {
        binding = aBinding;

        return *this;
    }

    DescriptorSetLayoutBinding& SetDescriptorType(DescriptorType aDescriptorType, uint32_t aDescriptorCount) noexcept
    {
        descriptorType  = aDescriptorType;
        descriptorCount = aDescriptorCount;

        return *this;
    }

    DescriptorSetLayoutBinding& SetShaderStageFlags(const ShaderStageFlags& aFlags) noexcept
    {
        stageFlags  = aFlags;

        return *this;
    }

    DescriptorSetLayoutBinding& SetImmutableSamplers(const Sampler* apImmultableSamplers) noexcept
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

    constexpr DescriptorSetLayoutCreateInfo(DescriptorSetLayoutBinding&&, const DescriptorSetLayoutCreateFlags& = DefaultFlags) noexcept = delete;

    DescriptorSetLayoutCreateInfo& SetBinding(DescriptorSetLayoutBinding&&) = delete;

public:
    const void*                         pNext{ nullptr };
    DescriptorSetLayoutCreateFlags      flags;
    uint32_t                            bindingCount{ 0 };
    const DescriptorSetLayoutBinding*   pBindings{ nullptr };

    DEFINE_CLASS_MEMBER(DescriptorSetLayoutCreateInfo)

    constexpr DescriptorSetLayoutCreateInfo(uint32_t aBindingCount, const DescriptorSetLayoutBinding* apSetLayoutBindings, const DescriptorSetLayoutCreateFlags& aFlags = DefaultFlags) noexcept
        : flags(aFlags), bindingCount(aBindingCount), pBindings(apSetLayoutBindings)
    {}

    constexpr DescriptorSetLayoutCreateInfo(const DescriptorSetLayoutBinding& aSetLayoutBinding, const DescriptorSetLayoutCreateFlags& aFlags = DefaultFlags) noexcept
        : flags(aFlags), bindingCount(1), pBindings(aSetLayoutBinding.AddressOf())
    {}

    template <typename B, typename = EnableIfValueType<ValueType<B>, DescriptorSetLayoutBinding>>
    constexpr DescriptorSetLayoutCreateInfo(B&& aSetLayoutBindings, const DescriptorSetLayoutCreateFlags& aFlags = DefaultFlags) noexcept
        : DescriptorSetLayoutCreateInfo(SizeOf<uint32_t>(aSetLayoutBindings), DataOf(aSetLayoutBindings), aFlags)
    {
        StaticLValueRefAssert(B, aSetLayoutBindings);
    }

    DescriptorSetLayoutCreateInfo& SetNext(const void* apNext) noexcept
    {
        pNext = apNext;

        return *this;
    }

    DescriptorSetLayoutCreateInfo& SetFlags(const DescriptorSetLayoutCreateFlags& aFlags) noexcept
    {
        flags = aFlags;

        return *this;
    }

    DescriptorSetLayoutCreateInfo& SetBindings(uint32_t aBindingCount, const DescriptorSetLayoutBinding* apSetLayoutBindings) noexcept
    {
        bindingCount    = aBindingCount;
        pBindings       = apSetLayoutBindings;

        return *this;
    }

    DescriptorSetLayoutCreateInfo& SetBinding(const DescriptorSetLayoutBinding& aSetLayoutBinding) noexcept
    {
        bindingCount    = 1;
        pBindings       = aSetLayoutBinding.AddressOf();

        return *this;
    }

    template <typename B, typename = EnableIfValueType<ValueType<B>, DescriptorSetLayoutBinding>>
    DescriptorSetLayoutCreateInfo& SetBindings(B&& aSetLayoutBindings) noexcept
    {
        StaticLValueRefAssert(B, aSetLayoutBindings);

        return SetBindings(SizeOf<uint32_t>(aSetLayoutBindings), DataOf(aSetLayoutBindings));
    }
};

ConsistencyCheck(DescriptorSetLayoutCreateInfo, pNext, flags, bindingCount, pBindings)



class DescriptorSetLayout : public internal::VkTrait<DescriptorSetLayout, VkDescriptorSetLayout>
{
private:
    VkDescriptorSetLayout mDescriptorSetLayout{ VK_NULL_HANDLE };

public:
    DescriptorSetLayout(void) noexcept = default;

    DescriptorSetLayout(std::nullptr_t) noexcept
    {}

    explicit DescriptorSetLayout(VkDescriptorSetLayout aDescriptorSetLayout) noexcept : mDescriptorSetLayout(aDescriptorSetLayout)
    {}
};

StaticSizeCheck(DescriptorSetLayout)



class DescriptorSet : public internal::VkTrait<DescriptorSet, VkDescriptorSet>
{
private:
    VkDescriptorSet mDescriptorSet{ VK_NULL_HANDLE };

public:
    DescriptorSet(void) noexcept = default;

    DescriptorSet(std::nullptr_t) noexcept
    {}

    explicit DescriptorSet(VkDescriptorSet aDescriptorSet) noexcept : mDescriptorSet(aDescriptorSet)
    {}
};

StaticSizeCheck(DescriptorSet)



struct DescriptorImageInfo : public internal::VkTrait<DescriptorImageInfo, VkDescriptorImageInfo>
{
    Sampler     sampler;
    ImageView   imageView;
    ImageLayout imageLayout;

    DEFINE_CLASS_MEMBER(DescriptorImageInfo)

    DescriptorImageInfo(const Sampler& aSampler, const ImageView& aImageView, const ImageLayout& aImageLayout) noexcept
        : sampler(aSampler), imageView(aImageView), imageLayout(aImageLayout)
    {}

    DescriptorImageInfo& SetSampler(const Sampler& aSampler) noexcept
    {
        sampler = aSampler;

        return *this;
    }

    DescriptorImageInfo& SetImageView(const ImageView& aImageView) noexcept
    {
        imageView = aImageView;

        return *this;
    }

    DescriptorImageInfo& SetImageLayout(const ImageLayout& aImageLayout) noexcept
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

    explicit constexpr DescriptorBufferInfo(const Buffer& aBuffer, DeviceSize aOffset = 0, DeviceSize aRange = VK_WHOLE_SIZE) noexcept
        : buffer(aBuffer), offset(aOffset), range(aRange)
    {}

    DescriptorBufferInfo& SetBuffer(const Buffer& aBuffer) noexcept
    {
        buffer = aBuffer;

        return *this;
    }

    DescriptorBufferInfo& SetExtent(DeviceSize aOffset, DeviceSize aRange) noexcept
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

    WriteDescriptorSetInfo(const DescriptorSet&, uint32_t, DescriptorType, DescriptorImageInfo&&) noexcept = delete;
    WriteDescriptorSetInfo(const DescriptorSet&, uint32_t, DescriptorType, DescriptorBufferInfo&&) noexcept = delete;
    WriteDescriptorSetInfo(const DescriptorSet&, uint32_t, DescriptorType, BufferView&&) noexcept = delete;

    WriteDescriptorSetInfo& SetImage(DescriptorImageInfo&&) noexcept = delete;
    WriteDescriptorSetInfo& SetBuffer(DescriptorBufferInfo&&) noexcept = delete;
    WriteDescriptorSetInfo& SetTexelBufferView(BufferView&&) noexcept = delete;

public:
    const void*                 pNext{ nullptr };
    DescriptorSet               dstSet;
    uint32_t                    dstBinding{ 0 };
    uint32_t                    dstArrayElement{ 0 };       // dstArrayElement is the starting element in the descriptor array.
    uint32_t                    descriptorCount{ 0 };       // descriptorCount is the number of descriptors to update (the number of elements in pImageInfo, pBufferInfo, pTexelBufferView).
    DescriptorType              descriptorType;
    const DescriptorImageInfo*  pImageInfo{ nullptr };
    const DescriptorBufferInfo* pBufferInfo{ nullptr };
    const BufferView*           pTexelBufferView{ nullptr };

    DEFINE_CLASS_MEMBER(WriteDescriptorSetInfo)

    WriteDescriptorSetInfo(const DescriptorSet& aDstSet, uint32_t aDstBinding, uint32_t aDstArrayElement, uint32_t aDescriptorCount, DescriptorType aDescriptorType,
        const DescriptorImageInfo* apImageInfo = nullptr, const DescriptorBufferInfo* apBufferInfo = nullptr, const BufferView* apTexelBufferView = nullptr) noexcept
        : dstSet(aDstSet), dstBinding(aDstBinding), dstArrayElement(aDstArrayElement), descriptorCount(aDescriptorCount), descriptorType(aDescriptorType),
          pImageInfo(apImageInfo), pBufferInfo(apBufferInfo), pTexelBufferView(apTexelBufferView)
    {}

    WriteDescriptorSetInfo(const DescriptorSet& aDstSet, uint32_t aDstBinding, DescriptorType aDescriptorType, const DescriptorImageInfo& aImageInfo) noexcept
        : WriteDescriptorSetInfo(aDstSet, aDstBinding, 0, 1, aDescriptorType, aImageInfo.AddressOf())
    {}

    WriteDescriptorSetInfo(const DescriptorSet& aDstSet, uint32_t aDstBinding, DescriptorType aDescriptorType, const DescriptorBufferInfo& aBufferInfo) noexcept
        : WriteDescriptorSetInfo(aDstSet, aDstBinding, 0, 1, aDescriptorType, nullptr, aBufferInfo.AddressOf())
    {}

    WriteDescriptorSetInfo(const DescriptorSet& aDstSet, uint32_t aDstBinding, DescriptorType aDescriptorType, const BufferView& aTexelBufferView) noexcept
        : WriteDescriptorSetInfo(aDstSet, aDstBinding, 0, 1, aDescriptorType, nullptr, nullptr, aTexelBufferView.AddressOf())
    {}

    WriteDescriptorSetInfo& SetNext(const void* apNext) noexcept
    {
        pNext = apNext;

        return *this;
    }

    WriteDescriptorSetInfo& SetDescriptorSet(const DescriptorSet& aDstSet) noexcept
    {
        dstSet = aDstSet;

        return *this;
    }

    WriteDescriptorSetInfo& SetBinding(uint32_t aBinding, uint32_t aDstArrayElement = 0, uint32_t aDescriptorCount = 1) noexcept
    {
        dstBinding      = aBinding;
        dstArrayElement = aDstArrayElement;
        descriptorCount = aDescriptorCount;

        return *this;
    }

    WriteDescriptorSetInfo& SetDescriptorType(DescriptorType aDescriptorType) noexcept
    {
        descriptorType = aDescriptorType;

        return *this;
    }

    // TODO: apImageInfo points to an array.
    WriteDescriptorSetInfo& SetImage(const DescriptorImageInfo* apImageInfo) noexcept
    {
        pImageInfo = apImageInfo;

        return *this;
    }

    WriteDescriptorSetInfo& SetImage(const DescriptorImageInfo& aImageInfo) noexcept
    {
        return SetImage(aImageInfo.AddressOf());
    }

    WriteDescriptorSetInfo& SetBuffer(const DescriptorBufferInfo* apBufferInfo) noexcept
    {
        pBufferInfo = apBufferInfo;

        return *this;
    }

    WriteDescriptorSetInfo& SetBuffer(const DescriptorBufferInfo& aBufferInfo) noexcept
    {
        return SetBuffer(aBufferInfo.AddressOf());
    }

    WriteDescriptorSetInfo& SetTexelBufferView(const BufferView* apTexelBufferView) noexcept
    {
        pTexelBufferView = apTexelBufferView;

        return *this;
    }

    WriteDescriptorSetInfo& SetTexelBufferView(const BufferView& aTexelBufferView) noexcept
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
        const DescriptorSet& aDstSet, uint32_t aDstBinding, uint32_t aDstArrayElement, uint32_t aDescriptorCount) noexcept
        : srcSet(aSrcSet), srcBinding(aSrcBinding), srcArrayElement(aSrcArrayElement),
          dstSet(aDstSet), dstBinding(aDstBinding), dstArrayElement(aDstArrayElement), descriptorCount(aDescriptorCount)
    {}

    CopyDescriptorSetInfo& SetSrcDescriptorSet(const DescriptorSet& aSrcSet, uint32_t aSrcBinding, uint32_t aSrcArrayElement) noexcept
    {
        srcSet          = aSrcSet;
        srcBinding      = aSrcBinding;
        srcArrayElement = aSrcArrayElement;

        return *this;
    }

    CopyDescriptorSetInfo& SetDstDescriptorSet(const DescriptorSet& aDstSet, uint32_t aDstBinding, uint32_t aDstArrayElement) noexcept
    {
        dstSet          = aDstSet;
        dstBinding      = aDstBinding;
        dstArrayElement = aDstArrayElement;

        return *this;
    }

    CopyDescriptorSetInfo& SetDescriptorCount(uint32_t aDescriptorCount) noexcept
    {
        descriptorCount = aDescriptorCount;

        return *this;
    }
};

ConsistencyCheck(CopyDescriptorSetInfo, pNext, srcSet, srcBinding, srcArrayElement, dstSet, dstBinding, dstArrayElement, descriptorCount)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_DESCRIPTOR_SET_H__
