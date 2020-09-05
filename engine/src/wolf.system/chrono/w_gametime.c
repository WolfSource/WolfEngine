#include "w_gametime.h"
#include "w_chrono.h"
#include <math.h>

typedef struct w_gametime_t
{
    double          last_time;
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
} w_gametime_t;

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

w_gametime w_gametime_init(_Inout_ w_mem_pool pMemPool)
{
    w_gametime _gametime = (w_gametime_t*)w_malloc(pMemPool, sizeof(struct w_gametime_t));
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
    _gametime->max_delta = 313918.0;

    //reset it
    w_game_time_reset(_gametime);

    return _gametime;
}

void w_game_time_reset(_Inout_ w_gametime pGameTime)
{
    if (!pGameTime)
    {
        return;
    }

    pGameTime->last_time = w_chrono_now_in_sec();
    pGameTime->left_over_ticks = 0;
    pGameTime->fps = 0;
    pGameTime->frames_this_second = 0;
    pGameTime->seconds_counter = 0;
}

uint64_t w_gametime_get_elapsed_ticks(_In_ w_gametime pGameTime)
{
    if (!pGameTime)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_t");
        return 0;
    }
    return pGameTime->elapsed_ticks;
}

double w_gametime_get_elapsed_seconds(_In_ w_gametime pGameTime)
{
    if (!pGameTime)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_t");
        return 0;
    }
    return ticks_to_seconds(pGameTime->elapsed_ticks);
}

uint64_t w_gametime_get_total_ticks(_In_ w_gametime pGameTime)
{
    if (!pGameTime)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_t");
        return 0;
    }
    return pGameTime->total_ticks;
}

double w_gametime_get_total_seconds(_In_ w_gametime pGameTime)
{
    if (!pGameTime)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_t");
        return 0;
    }
    return ticks_to_seconds(pGameTime->total_ticks);
}

uint32_t w_gametime_get_frame_count(_In_ w_gametime pGameTime)
{
    if (!pGameTime)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_t");
        return 0;
    }
    return pGameTime->frame_count;
}

uint32_t w_gametime_get_frames_per_second(_In_ w_gametime pGameTime)
{
    if (!pGameTime)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_t");
        return 0;
    }
    return pGameTime->fps;
}

bool w_gametime_get_fixed_time_step(_In_ w_gametime pGameTime)
{
    if (!pGameTime)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_t");
        return 0;
    }
    return pGameTime->fixed_time_step;
}

void w_gametime_enable_fixed_time_step(_In_ w_gametime pGameTime)
{
    if (!pGameTime)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_t");
        return;
    }
    pGameTime->fixed_time_step = true;
}

void w_gametime_disable_fixed_time_step(_In_ w_gametime pGameTime)
{
    if (!pGameTime)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_t");
        return;
    }
    pGameTime->fixed_time_step = false;
}

void w_gametime_set_target_elapsed_ticks(_In_ w_gametime pGameTime, _In_ uint64_t pValue)
{
    if (!pGameTime)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_t");
        return;
    }
    pGameTime->target_elapsed_ticks = pValue;
}

void w_gametime_set_target_elapsed_seconds(_In_ w_gametime pGameTime, _In_ double pValue)
{
    if (!pGameTime)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_t");
        return;
    }
    pGameTime->target_elapsed_ticks = seconds_to_ticks(pValue);
}


void w_gametime_tick(_In_ w_gametime pGameTime, _In_ w_gametime_tick_callback pOnTickCallBack)
{
    if (!pGameTime)
    {
        W_ASSERT(false, "could not convert w_gametime to w_gametime_t");
        return;
    }

    // Query the current time.
    double _current_time = w_chrono_now_in_sec();
    double _time_delta = _current_time - pGameTime->last_time;

    pGameTime->last_time = _current_time;
    pGameTime->seconds_counter += _time_delta;

    //clamp excessively large time deltas (e.g. after paused in the debugger).
    if (_time_delta > pGameTime->max_delta)
    {
        _time_delta = pGameTime->max_delta;
    }

    // Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
    _time_delta = _time_delta * TICKS_PER_SECOND;

    uint32_t _last_frame_count = pGameTime->frame_count;
    if (pGameTime->fixed_time_step)
    {
        /*
            If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
            the clock to exactly match the target value. This prevents tiny and irrelevant errors
            from accumulating over time. Without this clamping, a game that requested a 60 fps
            fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
            accumulate enough tiny errors that it would drop a frame. It is better to just round
            small deviations down to zero to leave things running smoothly.
        */

        if (llabs((int64_t)(_time_delta - pGameTime->target_elapsed_ticks)) < TICKS_PER_SECOND / 4000)
        {
            _time_delta = (double)pGameTime->target_elapsed_ticks;
        }

        pGameTime->left_over_ticks += (uint64_t)_time_delta;
        while (pGameTime->left_over_ticks >= pGameTime->target_elapsed_ticks)
        {
            pGameTime->elapsed_ticks = pGameTime->target_elapsed_ticks;
            pGameTime->total_ticks += pGameTime->target_elapsed_ticks;
            pGameTime->left_over_ticks -= pGameTime->target_elapsed_ticks;
            pGameTime->frame_count++;

            if (pOnTickCallBack)
            {
                pOnTickCallBack(pGameTime);
            }
        }
    }
    else
    {
        // Variable timestep update logic.
        pGameTime->elapsed_ticks = (uint64_t)_time_delta;
        pGameTime->total_ticks += (uint64_t)_time_delta;
        pGameTime->left_over_ticks = 0;
        pGameTime->frame_count++;

        if (pOnTickCallBack)
        {
            pOnTickCallBack(pGameTime);
        }
    }
    // Track the current framerate.
    pGameTime->frames_this_second += (pGameTime->frame_count - _last_frame_count);

    if (pGameTime->seconds_counter >= 1)
    {
        pGameTime->fps = pGameTime->frames_this_second;
        pGameTime->frames_this_second = 0;
        pGameTime->seconds_counter = fmod(pGameTime->seconds_counter, (double)1);
    }
}