#include "w_gametime.h"
#include "w_chrono.h"
#include <math.h>

typedef struct
{
#ifdef W_PLATFORM_WIN

    LARGE_INTEGER   frequency;
    LARGE_INTEGER   last_time;
    
#else
    
    double          last_time;
    
#endif
    
    double          max_delta;

    //derived timing data uses a canonical tick format.
    uint64_t        elapsed_ticks;
    uint64_t        total_ticks;
    uint64_t        left_over_ticks;

    //members for tracking the framerate.
    uint32_t        frame_count;
    uint32_t        fps;
    uint32_t        frames_this_second;
    double          seconds_counter;

    //members for configuring fixed timestep mode.
    bool            fixed_time_step;
    uint64_t        target_elapsed_ticks;
    
} w_gametime_imp;

//integer format represents time using 10,000,000 ticks per second.
static const double TICKS_PER_SECOND = 10000000;

static double ticks_to_seconds(uint64_t pTicks)
{
    return (double)pTicks / TICKS_PER_SECOND;
}

static uint64_t seconds_to_ticks(double pSeconds)
{
    return (uint64_t)(pSeconds * TICKS_PER_SECOND);
}

w_gametime w_gametime_init(void)
{
    w_gametime_imp* _gametime = (w_gametime_imp*)(w_alloc(sizeof(w_gametime_imp), "w_gametime_init"));
    if (!_gametime)
    {
        return NULL;
    }
    
    //initialize it
    _gametime->elapsed_ticks = 0;
    _gametime->total_ticks = 0;
    _gametime->frame_count = 0;
    _gametime->fixed_time_step = false;
    _gametime->target_elapsed_ticks = (uint64_t)(TICKS_PER_SECOND / 60);
    
#if defined(W_PLATFORM_WIN)
    //Get frequency
    if (!QueryPerformanceFrequency(&this->_frequency))
    {
                    V(W_FAILED,
                        w_log_type::W_ERROR,
                        true,
                        "query performance frequency (on constructor). trace info: {}", this->_name);
    }
    
    //Get performance
    if (!QueryPerformanceCounter(&this->_last_time))
    {
                    V(W_FAILED,
                        w_log_type::W_ERROR,
                        true,
                        "query performance frequency (on constructor). trace info: {}", this->_name);
    }
    
    // Initialize max delta to 1/10 of a second.
    _gametime->max_delta = (double)(this->_frequency.QuadPart / 10);
    
#elif defined(W_PLATFORM_ANDROID) || defined(W_PLATFORM_LINUX) || defined(W_PLATFORM_OSX)
    
    _gametime->max_delta = 313918.0;
    
#endif
    
    //reset it
    w_game_time_reset((w_gametime)_gametime);
    
    return (w_gametime)_gametime;
}

void w_game_time_reset(_In_ w_gametime pGameTime)
{
    if (!pGameTime)
    {
        return;
    }
    
    w_gametime_imp* _ptr = (w_gametime_imp*)pGameTime;
    
    _ptr->last_time = w_chrono_now_in_sec();
    _ptr->left_over_ticks = 0;
    _ptr->fps = 0;
    _ptr->frames_this_second = 0;
    _ptr->seconds_counter = 0;
}

uint64_t w_gametime_get_elapsed_ticks(_In_ w_gametime pGameTime)
{
    w_gametime_imp* _ptr = (w_gametime_imp*)pGameTime;
    if (!_ptr)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_imp");
        return 0;
    }
    return _ptr->elapsed_ticks;
}

double w_gametime_get_elapsvoided_seconds(_In_ w_gametime pGameTime)
{
    w_gametime_imp* _ptr = (w_gametime_imp*)pGameTime;
    if (!_ptr)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_imp");
        return 0;
    }
    return _ptr->elapsed_ticks;
}

uint64_t w_gametime_get_total_ticks(_In_ w_gametime pGameTime)
{
    w_gametime_imp* _ptr = (w_gametime_imp*)pGameTime;
    if (!_ptr)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_imp");
        return 0;
    }
    return _ptr->total_ticks;
}

double w_gametime_get_total_seconds(_In_ w_gametime pGameTime)
{
    w_gametime_imp* _ptr = (w_gametime_imp*)pGameTime;
    if (!_ptr)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_imp");
        return 0;
    }
    return ticks_to_seconds(_ptr->total_ticks);
}

uint32_t w_gametime_get_frame_count(_In_ w_gametime pGameTime)
{
    w_gametime_imp* _ptr = (w_gametime_imp*)pGameTime;
    if (!_ptr)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_imp");
        return 0;
    }
    return _ptr->frame_count;
}

uint32_t w_gametime_get_frames_per_second(_In_ w_gametime pGameTime)
{
    w_gametime_imp* _ptr = (w_gametime_imp*)pGameTime;
    if (!_ptr)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_imp");
        return 0;
    }
    return _ptr->fps;
}

bool w_gametime_get_fixed_time_step(_In_ w_gametime pGameTime)
{
    w_gametime_imp* _ptr = (w_gametime_imp*)pGameTime;
    if (!_ptr)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_imp");
        return 0;
    }
    return _ptr->fixed_time_step;
}

void w_gametime_enable_fixed_time_step(_In_ w_gametime pGameTime)
{
    w_gametime_imp* _ptr = (w_gametime_imp*)pGameTime;
    if (!_ptr)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_imp");
        return;
    }
    _ptr->fixed_time_step = true;
}

void w_gametime_disable_fixed_time_step(_In_ w_gametime pGameTime)
{
    w_gametime_imp* _ptr = (w_gametime_imp*)pGameTime;
    if (!_ptr)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_imp");
        return;
    }
    _ptr->fixed_time_step = false;
}

void w_gametime_set_target_elapsed_ticks(_In_ w_gametime pGameTime, _In_ uint64_t pValue)
{
    w_gametime_imp* _ptr = (w_gametime_imp*)pGameTime;
    if (!_ptr)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_imp");
        return;
    }
    _ptr->target_elapsed_ticks = pValue;
}

void w_gametime_set_target_elapsed_seconds(_In_ w_gametime pGameTime, _In_ double pValue)
{
    w_gametime_imp* _ptr = (w_gametime_imp*)pGameTime;
    if (!_ptr)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_imp");
        return;
    }
    _ptr->target_elapsed_ticks = seconds_to_ticks(pValue);
}


void w_gametime_tick(_In_ w_gametime pGameTime, _In_ w_gametime_tick_callback pOnTickCallBack)
{
    w_gametime_imp* _ptr = (w_gametime_imp*)pGameTime;
    if (!_ptr)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_imp");
        return;
    }
    
    // Query the current time.
    double _current_time = w_chrono_now_in_sec();
    double _time_delta = _current_time - _ptr->last_time;
    
    _ptr->last_time = _current_time;
    _ptr->seconds_counter += _time_delta;
    
    //clamp excessively large time deltas (e.g. after paused in the debugger).
    if (_time_delta > _ptr->max_delta)
    {
        _time_delta = (long long)(_ptr->max_delta);
    }
    
    // Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
    _time_delta = (long long)(_time_delta * TICKS_PER_SECOND);
    
    uint32_t _last_frame_count = _ptr->frame_count;
    if (_ptr->fixed_time_step)
    {
        /*
            If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
            the clock to exactly match the target value. This prevents tiny and irrelevant errors
            from accumulating over time. Without this clamping, a game that requested a 60 fps
            fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
            accumulate enough tiny errors that it would drop a frame. It is better to just round
            small deviations down to zero to leave things running smoothly.
        */
    
        if (llabs((int64_t)(_time_delta - _ptr->target_elapsed_ticks)) < TICKS_PER_SECOND / 4000)
        {
            _time_delta = _ptr->target_elapsed_ticks;
        }
    
        _ptr->left_over_ticks += _time_delta;
        while (_ptr->left_over_ticks >= _ptr->target_elapsed_ticks)
        {
            _ptr->elapsed_ticks = _ptr->target_elapsed_ticks;
            _ptr->total_ticks += _ptr->target_elapsed_ticks;
            _ptr->left_over_ticks -= _ptr->target_elapsed_ticks;
            _ptr->frame_count++;
            
            if (pOnTickCallBack)
            {
                pOnTickCallBack(pGameTime);
            }
        }
    }
    else
    {
        // Variable timestep update logic.
        _ptr->elapsed_ticks = (uint64_t)_time_delta;
        _ptr->total_ticks += (uint64_t)_time_delta;
        _ptr->left_over_ticks = 0;
        _ptr->frame_count++;
    
        if (pOnTickCallBack)
        {
            pOnTickCallBack(pGameTime);
        }
    }
    // Track the current framerate.
    _ptr->frames_this_second += (_ptr->frame_count - _last_frame_count);
    
    if (_ptr->seconds_counter >= 1)
    {
        _ptr->fps = _ptr->frames_this_second;
        _ptr->frames_this_second = 0;
        _ptr->seconds_counter = fmod(_ptr->seconds_counter, (double)1);
    }
}




