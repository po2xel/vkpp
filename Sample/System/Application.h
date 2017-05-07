#ifndef __VKPP_SAMPLE_SYSTEM_DMAPPLICATION_H__
#define __VKPP_SAMPLE_SYSTEM_DMAPPLICATION_H__



#include <functional>
#include <unordered_map>

#include <SDL2/SDL.h>

#include <System/Singleton.h>
#include <System/Exception.h>



namespace vkpp::sample
{



using WindowEvent       = SDL_WindowEvent;
using MouseButtonEvent  = SDL_MouseButtonEvent;
using MouseMotionEvent  = SDL_MouseMotionEvent;
using MouseWheelEvent   = SDL_MouseWheelEvent;
using TouchFingerEvent  = SDL_TouchFingerEvent;
using MultiGestureEvent = SDL_MultiGestureEvent;
using KeyboardEvent     = SDL_KeyboardEvent;
using UserEvent         = SDL_UserEvent;
using KeyCode           = SDL_Keycode;



class CWindowEvent;
class CKeyboardEvent;
class CMouseButtonEvent;
class CMouseMotionEvent;
class CMouseWheelEvent;
class CTouchFingerEvent;
class CMultiGestureEvent;
class CRedrawEvent;



struct EventHandle
{
    std::function<void(const WindowEvent&)> mWindowEventHandle = [](const WindowEvent&) {};

    std::function<void(const KeyboardEvent&)> mKeyDownEventHandle = [](const KeyboardEvent&) {};
    std::function<void(const KeyboardEvent&)> mKeyUpEventHandle = [](const KeyboardEvent&) {};

    std::function<void(const MouseButtonEvent&)> mButtonDownEventHandle = [](const MouseButtonEvent&) {};
    std::function<void(const MouseButtonEvent&)> mButtonUpEventHandle = [](const MouseButtonEvent&) {};
    std::function<void(const MouseMotionEvent&)> mMouseMotionEventHandle = [](const MouseMotionEvent&) {};
    std::function<void(const MouseWheelEvent&) > mMouseWheelEventHandle = [](const MouseWheelEvent&) {};

    std::function<void(void)> mRedrawEventHandle = [](void) {};
};



struct TouchEventHandle
{
    std::function<void(const TouchFingerEvent&)> mFingerDownEventHandle = [](const TouchFingerEvent&) {};
    std::function<void(const TouchFingerEvent&)> mFingerUpEventHandle = [](const TouchFingerEvent&) {};
    std::function<void(const TouchFingerEvent&)> mFingerMotionEventHandle = [](const TouchFingerEvent&) {};

    std::function<void(const MultiGestureEvent&)> mMultiGestureEventHanle = [](const MultiGestureEvent&) {};
};



class CApplication : public CSingleton<CApplication>
{
protected:
    std::unordered_map<Uint32, EventHandle> mEventHandler;
    TouchEventHandle mTouchEventHandler;                    // Only one touch device is supported.
    // mutable std::unordered_map<SDL_TouchID, TouchEventHandle> mTouchEventHandler;

    std::vector<std::function<void(void)>> mUpdateFunc;

public:
    enum SubSystem
    {
        TIMER = SDL_INIT_TIMER,
        AUDIO = SDL_INIT_AUDIO,
        VIDEO = SDL_INIT_VIDEO,
        JOYSTICK = SDL_INIT_JOYSTICK,
        HAPTIC = SDL_INIT_HAPTIC,
        GAMECONTROLLER = SDL_INIT_GAMECONTROLLER,
        EVENTS = SDL_INIT_EVENTS,
        EVERYTHING = SDL_INIT_EVERYTHING
    };

    explicit CApplication(SubSystem aFlags)
    {
        if (SDL_Init(aFlags) != 0)
            throw CInitException();
    }

    virtual ~CApplication(void)
    {
        SDL_Quit();
    }

    static bool InitSubSystem(SubSystem aFlags)
    {
        return 0 == SDL_InitSubSystem(aFlags);
    }

    static void QuitSubSystem(SubSystem aFlags)
    {
        SDL_QuitSubSystem(aFlags);
    }

    static bool IsInitialized(SubSystem aFlags)
    {
        return SDL_WasInit(aFlags) != 0;
    }

    void RegisterWindowEvent(const CWindowEvent& aRegWin);
    void UnRegisterWindowEvent(const CWindowEvent& aRegWin);

    void RegisterKeyboardEvent(const CKeyboardEvent& aRegKeyboard);

    void RegisterMouseEvent(const CMouseButtonEvent& aRegButton);
    void RegisterMouseMotionEvent(const CMouseMotionEvent& aRegMotion);
    void RegisterMouseWheelEvent(const CMouseWheelEvent& aRegWheel);

    void RegisterRedrawEvent(const CRedrawEvent& aRegRedraw);

    void RegisterUpdateEvent(std::function<void(void)> aUpdateFunc);

    void RegisterTouchFingerEvent(const CTouchFingerEvent& aRegFinger);
    void RegisterMultiGestureEent(const CMultiGestureEvent& aMultiGesture);

    virtual void Run(void) const;
};



using TheApp = CApplication;
extern CApplication& theApp;



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_SYSTEM_DMAPPLICATION_H__