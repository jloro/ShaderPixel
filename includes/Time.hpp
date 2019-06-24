#ifndef TIME_HPP
# define TIME_HPP
# include "SDL.h"

namespace Engine42
{
    class Time 
    {
    public: 
    /*	constructors / destrucors	*/
        virtual ~Time(void); 

        /*	public variables	*/
        /*	public functions	*/
        static float   GetDeltaTime(void);
        static float   GetFixedDeltaTime(void);
        static void    SetDeltaTime(float time);
        static void    SetFixedDeltaTime(float fixedTime);

    private:
    /*  private constructors    */
        Time(void); 
    /*	private variables	*/
        static Time     _inst;
        float           _deltaTime;
        float           _fixedDeltaTime;
    /*	private functions	*/
    };
}

#endif