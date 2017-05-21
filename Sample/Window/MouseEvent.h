#ifndef __VKPP_SAMPLE_WINDOW_MOUSEEVENT_H__
#define __VKPP_SAMPLE_WINDOW_MOUSEEVENT_H__



#include <cassert>

#include <Window/Event.h>



namespace vkpp::sample
{



class CMouseEvent : public CEvent
{
public:
    explicit CMouseEvent(const CWindow& aWin) : CEvent(aWin)
    {}

    virtual ~CMouseEvent(void) = default;

    enum Button
    {
        eLeft       = SDL_BUTTON_LEFT,
        eRight      = SDL_BUTTON_RIGHT,
        eMiddle     = SDL_BUTTON_MIDDLE
    };

    enum ButtonMask
    {
        eLMask       = SDL_BUTTON_LMASK,
        eRMask       = SDL_BUTTON_RMASK,
        eMMask       = SDL_BUTTON_MMASK
    };
};



class CMouseButtonEvent : public CMouseEvent
{
protected:
    std::function<void(Sint32 aPosX, Sint32 aPosY)> mLButtonDownFunc;
    std::function<void(Sint32 aPosX, Sint32 aPosY)> mLButtonUpFunc;

    std::function<void(Sint32 aPosX, Sint32 aPosY)> mRButtonDownFunc;
    std::function<void(Sint32 aPosX, Sint32 aPosY)> mRButtonUpFunc;

    std::function<void(Sint32 aPosX, Sint32 aPosY)> mMButtonDownFunc;
    std::function<void(Sint32 aPosX, Sint32 aPosY)> mMButtonUpFunc;

    explicit CMouseButtonEvent(const CWindow& aWin) : CMouseEvent(aWin)
    {
        theApp.RegisterMouseEvent(*this);
    }

    ~CMouseButtonEvent(void) = default;

public:
    void OnButtonDownEvent(const MouseButtonEvent& aEvent) const 
    {
        assert(SDL_MOUSEBUTTONDOWN == aEvent.type);

        if (mLButtonDownFunc && (SDL_BUTTON_LEFT == aEvent.button) != 0)
            mLButtonDownFunc(aEvent.x, aEvent.y);
        else if (mRButtonDownFunc && (SDL_BUTTON_RIGHT == aEvent.button) != 0)
            mRButtonDownFunc(aEvent.x, aEvent.y);
        else if (mMButtonDownFunc && (SDL_BUTTON_MIDDLE == aEvent.button) != 0)
            mMButtonDownFunc(aEvent.x, aEvent.y);
    }

    void OnButtonUpEvent(const MouseButtonEvent& aEvent) const
    {
        assert(SDL_MOUSEBUTTONUP == aEvent.type);

        if (mLButtonUpFunc && (SDL_BUTTON_LEFT == aEvent.button) != 0)
            mLButtonUpFunc(aEvent.x, aEvent.y);
        else if (mRButtonUpFunc && (SDL_BUTTON_RIGHT == aEvent.button) != 0)
            mRButtonUpFunc(aEvent.x, aEvent.y);
        else if (mMButtonUpFunc && (SDL_BUTTON_MIDDLE == aEvent.button) != 0)
            mMButtonUpFunc(aEvent.x, aEvent.y);
    }
};



class CMouseMotionEvent : public CMouseEvent
{
protected:
    std::function<void(Uint32 aButtonMask, Sint32 aPosX, Sint32 aPosY, Sint32 aPosXRel, Sint32 aPosYRel)> mMouseMotionFunc;

    explicit CMouseMotionEvent(const CWindow& aWin) : CMouseEvent(aWin)
    {
        theApp.RegisterMouseMotionEvent(*this);
    }

    ~CMouseMotionEvent(void) = default;

public:
    void OnMotionEvent(const MouseMotionEvent& aEvent) const
    {
        assert(SDL_MOUSEMOTION == aEvent.type);

        if (mMouseMotionFunc)
            mMouseMotionFunc(aEvent.state, aEvent.x, aEvent.y, aEvent.xrel, aEvent.yrel);
    }
};



class CMouseWheelEvent : public CMouseEvent
{
protected:
    std::function<void(Sint32 aPosX, Sint32 aPosY)> mMouseWheelFunc;

    explicit CMouseWheelEvent(const CWindow& aWin) : CMouseEvent(aWin)
    {
        theApp.RegisterMouseWheelEvent(*this);
    }

    ~CMouseWheelEvent(void) = default;

public:
    void OnWheelEvent(const MouseWheelEvent& aEvent) const
    {
        assert(SDL_MOUSEWHEEL == aEvent.type);

        if (mMouseWheelFunc)
            mMouseWheelFunc(aEvent.x, aEvent.y);
    }
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_WINDOW_MOUSEEVENT_H__