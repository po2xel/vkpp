#ifndef __VKPP_SAMPLE_WINDOW_KEYBOARD_EVENT_H__
#define __VKPP_SAMPLE_WINDOW_KEYBOARD_EVENT_H__



#include <Window/Event.h>



namespace vkpp::sample
{



class CKeyboardEvent : public CEvent
{
protected:
    std::function<void(const KeyCode&)> mKeyDownFunc;
    std::function<void(const KeyCode&)> mKeyUpFunc;

    explicit CKeyboardEvent(const CWindow& aWin) : CEvent(aWin)
    {
        theApp.RegisterKeyboardEvent(*this);
    }

    ~CKeyboardEvent(void) = default;

public:
    void OnKeyDownEvent(const KeyboardEvent& aEvent) const
    {
        assert(SDL_KEYDOWN == aEvent.type);

        if(mKeyDownFunc)
            mKeyDownFunc(aEvent.keysym.sym);
    }

    void OnKeyUpEvent(const KeyboardEvent& aEvent) const
    {
        assert(SDL_KEYUP == aEvent.type);

        if (mKeyUpFunc)
            mKeyUpFunc(aEvent.keysym.sym);
    }
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_WINDOW_KEYBOARD_EVENT_H__