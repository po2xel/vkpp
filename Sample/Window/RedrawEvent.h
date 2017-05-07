#ifndef __VKPP_SAMPLE_REDRAW_EVENT_H__
#define __VKPP_SAMPLE_REDRAW_EVENT_H__



#include <Window/Event.h>



namespace vkpp::sample
{



class CRedrawEvent : public CEvent
{
protected:
    std::function<void()> mRedrawFunc;

    explicit CRedrawEvent(const CWindow& aWin) : CEvent(aWin)
    {
        theApp.RegisterRedrawEvent(*this);
    }

    ~CRedrawEvent(void) = default;

public:
    void OnRedraw(void) const
    {
        if (mRedrawFunc)
            mRedrawFunc();
    }

    static void Clear(void)
    {
        SDL_FlushEvent(USER_WM_REDRAW);
    }
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_REDRAW_EVENT_H__