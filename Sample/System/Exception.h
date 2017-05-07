#ifndef __VKPP_SAMPLE_SYSTEM_EXCEPTION_H__
#define __VKPP_SAMPLE_SYSTEM_EXCEPTION_H__



#include <exception>
#include <string>

#include <SDL2/SDL_error.h>




namespace vkpp::sample
{



class CException : public std::exception
{
private:
    std::string mMsg;

public:
    CException(void) : mMsg(SDL_GetError())
    {}

    explicit CException(const std::string& aMsg) : mMsg(aMsg)
    {}

    virtual ~CException(void) noexcept
    {}

    virtual const char* what(void) const noexcept override
    {
        return mMsg.c_str();
    }
};



class CInitException : public CException
{};



class CWindowCreationException : public CException
{};



}                   // End of namespace dm



#endif              // __DM_SYSTEM_EXCEPTION_H__