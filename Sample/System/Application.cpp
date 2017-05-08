#include <System/Application.h>
#include <Window/WindowEvent.h>
#include <Window/MouseEvent.h>
// #include <Window/TouchFingerEvent.h>
#include <Window/KeyboardEvent.h>
#include <Window/RedrawEvent.h>



namespace vkpp::sample
{



void CApplication::RegisterWindowEvent(const CWindowEvent& aRegWin)
{
    mEventHandler[aRegWin.GetWindowID()].mWindowEventHandle = std::bind(&CWindowEvent::OnWindowEvent, std::cref(aRegWin), std::placeholders::_1);
}


void CApplication::UnRegisterWindowEvent(const CWindowEvent& aRegWin)
{
    mEventHandler[aRegWin.GetWindowID()].mWindowEventHandle = [](const WindowEvent&) {};
}


void CApplication::RegisterKeyboardEvent(const CKeyboardEvent& aRegKeyboard)
{
    const auto& lWinID = aRegKeyboard.GetWindowID();

    mEventHandler[lWinID].mKeyDownEventHandle = std::bind(&CKeyboardEvent::OnKeyDownEvent, std::cref(aRegKeyboard), std::placeholders::_1);
    mEventHandler[lWinID].mKeyUpEventHandle = std::bind(&CKeyboardEvent::OnKeyUpEvent, std::cref(aRegKeyboard), std::placeholders::_1);
}


void CApplication::RegisterMouseEvent(const CMouseButtonEvent& aRegButton)
{
    const auto& lWinID = aRegButton.GetWindowID();

    mEventHandler[lWinID].mButtonDownEventHandle = std::bind(&CMouseButtonEvent::OnButtonDownEvent, std::cref(aRegButton), std::placeholders::_1);
    mEventHandler[lWinID].mButtonUpEventHandle = std::bind(&CMouseButtonEvent::OnButtonUpEvent, std::cref(aRegButton), std::placeholders::_1);
}


void CApplication::RegisterMouseMotionEvent(const CMouseMotionEvent& aRegMotion)
{
    mEventHandler[aRegMotion.GetWindowID()].mMouseMotionEventHandle = std::bind(&CMouseMotionEvent::OnMotionEvent, std::cref(aRegMotion), std::placeholders::_1);
}


void CApplication::RegisterMouseWheelEvent(const CMouseWheelEvent& aRegWheel)
{
    mEventHandler[aRegWheel.GetWindowID()].mMouseWheelEventHandle = std::bind(&CMouseWheelEvent::OnWheelEvent, std::cref(aRegWheel), std::placeholders::_1);
}


void CApplication::RegisterRedrawEvent(const CRedrawEvent& aRegRedraw)
{
    mEventHandler[aRegRedraw.GetWindowID()].mRedrawEventHandle = std::bind(&CRedrawEvent::OnRedraw, std::cref(aRegRedraw));
}


void CApplication::RegisterUpdateEvent(std::function<void(void)> aUpdateFunc)
{
    mUpdateFunc.emplace_back(aUpdateFunc);
}


//void CApplication::RegisterTouchFingerEvent(const CTouchFingerEvent& aRegFinger)
//{
//    mTouchEventHandler.mFingerDownEventHandle = std::bind(&CTouchFingerEvent::OnFingerDownEvent, std::cref(aRegFinger), std::placeholders::_1);
//    mTouchEventHandler.mFingerUpEventHandle = std::bind(&CTouchFingerEvent::OnFingerUpEvent, std::cref(aRegFinger), std::placeholders::_1);
//    mTouchEventHandler.mFingerMotionEventHandle = std::bind(&CTouchFingerEvent::OnFingerMotionEvent, std::cref(aRegFinger), std::placeholders::_1);
//}
//
//
//void CApplication::RegisterMultiGestureEent(const CMultiGestureEvent& aMultiGesture)
//{
//    mTouchEventHandler.mMultiGestureEventHanle = std::bind(&CMultiGestureEvent::OnMultiGestureEvent, std::cref(aMultiGesture), std::placeholders::_1);
//}


void CApplication::Run(void) const
{
    SDL_Event lWinEvent;

    do
    {
        while(SDL_PollEvent(&lWinEvent))
        {
            auto lEventHandler = mEventHandler.find(lWinEvent.window.windowID);
            if (lEventHandler == mEventHandler.end())
                continue;

            switch (lWinEvent.type)
            {
            case SDL_WINDOWEVENT:
                mEventHandler.at(lWinEvent.window.windowID).mWindowEventHandle(lWinEvent.window);
                break;

            case SDL_KEYDOWN:
                mEventHandler.at(lWinEvent.key.windowID).mKeyDownEventHandle(lWinEvent.key);
                break;

            case SDL_KEYUP:
                mEventHandler.at(lWinEvent.key.windowID).mKeyUpEventHandle(lWinEvent.key);
                break;

            case SDL_MOUSEMOTION:
                mEventHandler.at(lWinEvent.motion.windowID).mMouseMotionEventHandle(lWinEvent.motion);
                break;

            case SDL_MOUSEWHEEL:
                mEventHandler.at(lWinEvent.wheel.windowID).mMouseWheelEventHandle(lWinEvent.wheel);
                break;

            case SDL_MOUSEBUTTONDOWN:
                mEventHandler.at(lWinEvent.button.windowID).mButtonDownEventHandle(lWinEvent.button);
                break;

            case SDL_MOUSEBUTTONUP:
                mEventHandler.at(lWinEvent.button.windowID).mButtonUpEventHandle(lWinEvent.button);
                break;

            /*case SDL_FINGERDOWN:
                mTouchEventHandler.mFingerDownEventHandle(lWinEvent.tfinger);
                break;

            case SDL_FINGERUP:
                mTouchEventHandler.mFingerUpEventHandle(lWinEvent.tfinger);
                break;

            case SDL_FINGERMOTION:
                mTouchEventHandler.mFingerMotionEventHandle(lWinEvent.tfinger);
                break;

            case SDL_MULTIGESTURE:
                mTouchEventHandler.mMultiGestureEventHanle(lWinEvent.mgesture);
                break;*/

            case USER_WM_REDRAW:
                mEventHandler.at(lWinEvent.button.windowID).mRedrawEventHandle();
                break;

            /*case SDL_QUIT:
                return;*/
            }
        }

        for (auto lUpdateFunc : mUpdateFunc)
            lUpdateFunc();

    } while (lWinEvent.type != SDL_QUIT);
}



}               // End of namespace vkpp::sample.