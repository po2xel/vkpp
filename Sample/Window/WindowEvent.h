#ifndef __VKPP_SAMPLE_WINDOW_WINDOW_EVENT_H__
#define __VKPP_SAMPLE_WINDOW_WINDOW_EVENT_H__



#include <Window/Event.h>



namespace vkpp::sample
{



class CWindowEvent : public CEvent
{
protected:
    explicit CWindowEvent(const CWindow& aWin) : CEvent(aWin)
    {
        theApp.RegisterWindowEvent(*this);
    }

    ~CWindowEvent(void)
    {
        theApp.UnRegisterWindowEvent(*this);
    }

    std::function<void(void)> mShowFunc;                    // Window has been shown.
    std::function<void(void)> mHideFunc;                    // Window has been hidden.
    std::function<void(void)> mExposedFunc;                 // Window has been exposed and should be redraw.

    std::function<void(Sint32 aPosX, Sint32 aPosY)> mMoveFunc;      // Window has been moved.
    std::function<void (Sint32 aPosX, Sint32 aPosY)> mResizedFunc;  // Window has been resized. This event is preceded by SIZE_CHANGED.
    std::function<void (void)> mSizeChangedFunc;                    // Window size has changed, either as a result of an API call or through the system or user
                                                                    // changing the window size. This event is followed by RESIZED if the size was changed by an
                                                                    // external event, i.e. the user or the window manager.
    
    std::function<void(void)> mMinimizedFunc;             // Window has been minimized.
    std::function<void(void)> mMaximizedFunc;             // Window has been maximized.
    std::function<void(void)> mRestoredFunc;              // Window has been restored to normal size and position.

    std::function<void(void)> mEnterFunc;                 // Window has gained mouse focus.
    std::function<void(void)> mLeaveFunc;                 // Window has lost mouse focus.
    std::function<void(void)> mFocusGainedFunc;           // Window has gained keyboard focus.
    std::function<void(void)> mFocusLostFunc;             // Window has lost keyboard focus.

    std::function<void(void)> mWindowCloseFunc;           // The window manager requests that the window be closed.

public:
    void OnWindowEvent(const WindowEvent& aEvent) const
    {
        switch(aEvent.event)
        {
        case SDL_WINDOWEVENT_SHOWN:
            if (mShowFunc)
                mShowFunc();
            break;

        case SDL_WINDOWEVENT_HIDDEN:
            if (mHideFunc)
                mHideFunc();
            break;

        case SDL_WINDOWEVENT_EXPOSED:
            if (mExposedFunc)
                mExposedFunc();
            break;

        case SDL_WINDOWEVENT_MOVED:
            if (mMoveFunc)
                mMoveFunc(aEvent.data1, aEvent.data2);
            break;

        case SDL_WINDOWEVENT_RESIZED:
            if (mResizedFunc)
                mResizedFunc(aEvent.data1, aEvent.data2);
            break;

        case SDL_WINDOWEVENT_SIZE_CHANGED:
            if (mSizeChangedFunc)
                mSizeChangedFunc();
            break;

        case SDL_WINDOWEVENT_MINIMIZED:
            if (mMinimizedFunc)
                mMinimizedFunc();
            break;

        case SDL_WINDOWEVENT_MAXIMIZED:
            if (mMaximizedFunc)
                mMaximizedFunc();
            break;

        case SDL_WINDOWEVENT_RESTORED:
            if (mRestoredFunc)
                mRestoredFunc();
            break;

        case SDL_WINDOWEVENT_ENTER:
            if (mEnterFunc)
                mEnterFunc();
            break;

        case SDL_WINDOWEVENT_LEAVE:
            if (mLeaveFunc)
                mLeaveFunc();
            break;

        case SDL_WINDOWEVENT_FOCUS_GAINED:
            if (mFocusGainedFunc)
                mFocusGainedFunc();
            break;

        case SDL_WINDOWEVENT_FOCUS_LOST:
            if (mFocusLostFunc)
                mFocusLostFunc();
            break;

        case SDL_WINDOWEVENT_CLOSE:
            if (mWindowCloseFunc)
                mWindowCloseFunc();
            break;
        }
    }
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_WINDOW_WINDOW_EVENT_H__