#include "Time.hpp"

Engine42::Time      Engine42::Time::_inst = Engine42::Time();

Engine42::Time::Time(void)
{
    _deltaTime = 0.0f;
    _fixedDeltaTime = 0.0f;
}

Engine42::Time::~Time(void) {}
/*  Getters */
float    Engine42::Time::GetDeltaTime(void){ return _inst._deltaTime;}
float    Engine42::Time::GetFixedDeltaTime(void){ return _inst._fixedDeltaTime; }
/*  Setters */
void     Engine42::Time::SetDeltaTime(float time){_inst._deltaTime = time;}
void    Engine42::Time::SetFixedDeltaTime(float fixedTime) {_inst._fixedDeltaTime = fixedTime;}