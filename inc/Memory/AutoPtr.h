#ifndef __VKPP_MEMORY_AUTOPTR_H__
#define __VKPP_MEMORY_AUTOPTR_H__



#include <type_traits>
#include <memory>
#include <vulkan/vulkan.h>



namespace vkpp
{



namespace internal
{



template <typename T, typename U>
struct VkDeleterTrait
{
    static_assert(std::is_pointer_v<T>);
    static_assert(std::is_pointer_v<U>);

    using DeleterType = std::add_pointer_t<void(T, U, const VkAllocationCallbacks*)>;

    T mpOwner{ nullptr };
};


template <typename T>
struct VkDeleterTrait<T, T>
{
    static_assert(std::is_pointer_v<T>);

    using DeleterType = std::add_pointer_t<void(T, const VkAllocationCallbacks*)>;
};



template <typename T, typename U, typename = std::enable_if_t<std::is_pointer_v<T>>>
class DefaultDeleter : public VkDeleterTrait<T, U>
{
private:
    using Base = VkDeleterTrait<T, U>;
    using DeleterType = typename Base::DeleterType;

    DeleterType mDeleter{ nullptr };
    const VkAllocationCallbacks* mpAllocator{ nullptr };

public:
    template <typename P, typename = std::enable_if_t<std::is_same_v<T, U>>>
    DefaultDeleter(P aDeleter, const VkAllocationCallbacks* apAllocator = nullptr) : mDeleter(aDeleter), mpAllocator(apAllocator)
    {
        assert(aDeleter != nullptr);
    }

    template <typename P, typename = std::enable_if_t<!std::is_same_v<T, U>>>
    DefaultDeleter(T apOwner, P aDeleter, const VkAllocationCallbacks* apAllocator = nullptr) : Base{ apOwner }, mDeleter(aDeleter), mpAllocator(apAllocator)
    {
        assert(apOwner != nullptr && aDeleter != nullptr);
    }

    template <typename P>
    std::enable_if_t<std::is_same_v<T, U>> operator()(P aPtr) const
    {
        mDeleter(aPtr, mpAllocator);
    }

    template <typename P>
    std::enable_if_t<!std::is_same_v<T, U>> operator()(P aPtr) const
    {
        assert(mpOwner != nullptr);

        mDeleter(mpOwner, aPtr, mpAllocator);
    }
};



}                   // End of namespace internal.



template <typename T>
using DefaultDeleter = internal::DefaultDeleter<T, T>;


template <typename T>
using InstanceDeleter = internal::DefaultDeleter<T, VkInstance>;


template <typename T>
using DeviceDeleter = internal::DefaultDeleter<T, VkDevice>;




template <typename T, typename Deleter>
class RefCounter
{
private:
    using ThisType = RefCounter;

    using RefType = T;
    using Reference = RefType&;
    using ConstReference = const Reference&;

    unsigned int mRefCount{ 0 };
    T mRef{ nullptr };
    Deleter mDeleter;

public:
    RefCounter(void) noexcept = default;

    template <typename... Args>
    explicit RefCounter(T apPtr, Args&&... aArgs) : mRef(apPtr), mDeleter(std::forward<Args>(aArgs)...)
    {
        AddRef();
    }

    ~RefCounter(void)
    {
        mDeleter(mRef);
    }

    void AddRef(void)
    {
        ++mRefCount;
    }

    void ReleaseRef(void)
    {
        if (--mRefCount == 0)
            delete this;
    }

    unsigned int RefCount(void) const
    {
        return mRefCount;
    }

    ConstReference Ref(void) const
    {
        return mRef;
    }

    Reference Ref(void)
    {
        return const_cast<Reference>(
            static_cast<const ThisType&>(*this).Ref());
    }

    operator ConstReference(void) const
    {
        return Ref();
    }

    operator Reference(void)
    {
        return Ref();
    }

    explicit operator bool(void) const
    {
        return mRef != nullptr;
    }

    RefType operator*(void) const
    {
        assert(mRef != nullptr);

        return mRef;
    }
};



template <typename T, typename Deleter = DefaultDeleter<T>>
class AutoPtr
{
private:
    using ElementType = T;
    using PointerType = T;
    using ThisType = AutoPtr;
    using RefCounterType = RefCounter<T, Deleter>;

    RefCounterType* mpRefCount{ nullptr };

public:
    AutoPtr(void) noexcept = default;

    /*explicit AutoPtr(std::nullptr_t) noexcept
    {}*/

    template <typename... Args>
    explicit AutoPtr(Args... aArgs) : mpRefCount(new RefCounterType(std::forward<Args>(aArgs)...))
    {}

    AutoPtr(const AutoPtr& aPtr) : mpRefCount(aPtr.mpRefCount)
    {
        mpRefCount->AddRef();
    }

    ~AutoPtr(void)
    {
        if (mpRefCount != nullptr)
            mpRefCount->ReleaseRef();
    }

    void Swap(AutoPtr& aPtr)
    {
        std::swap(mpRefCount, aPtr.mpRefCount);
    }

    AutoPtr& operator=(PointerType apPtr)
    {
        ThisType(apPtr).Swap(*this);

        return *this;
    }

    operator T(void) const
    {
        return *mpRefCount;
    }
};



}                   // End of namespace vkpp.



#endif              // __VKPP_MEMORY_AUTOPTR_H__
