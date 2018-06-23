/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to use fiber
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"
#include <w_thread.h>
#include <boost/fiber/all.hpp>
#include <w_timer.h>

//namespaces
using namespace std;
using namespace wolf;
using namespace wolf::system;

static size_t fiber_count = 0;
static std::mutex mutex_count;
static boost::fibers::condition_variable_any cv_count;

static int value1 = 0;
static int value2 = 0;

inline void fn( std::string const& pStr, int pSize)
{
    for ( int i = 0; i < pSize; ++i)
    {
        logger.write("{} : {}", i, pStr);
        boost::this_fiber::yield();
    }
}

inline void fn1(boost::fibers::barrier& b)
{
	auto _id = boost::this_fiber::get_id();
	ostringstream _buffer;
	_buffer << _id;
	auto _id_str = _buffer.str();
	_buffer.clear();

	logger.write("fiber {} fn1 entered", _id_str);

	++value1;
	logger.write("fiber {}: incremented value1: {} .", _id_str, value1);
	boost::this_fiber::yield();

	logger.write("current thread id of fn1: {} .", w_thread::get_current_thread_id());

	logger.write("fiber {} waits for barrier.", _id_str);
	b.wait();
	logger.write("fiber {} passed barrier.", _id_str);

	++value1;
	logger.write("fiber {} incremented value1: {}", _id_str, value1);
	boost::this_fiber::yield();

	++value1;
	logger.write("fiber {} incremented value1: {}", _id_str, value1);
	boost::this_fiber::yield();

	++value1;
	logger.write("fiber {} incremented value1: {}", _id_str, value1);
	boost::this_fiber::yield();

	logger.write("fiber {} fn1 returns", _id_str);
}

inline void fn2( boost::fibers::barrier & b)
{
	auto _id = boost::this_fiber::get_id();
	ostringstream _buffer;
	_buffer << _id;
	auto _id_str = _buffer.str();
	_buffer.clear();

    logger.write("fiber {} fn2 entered", _id_str);
    
    ++value2;
	logger.write("fiber {} incremented value2: {}", _id_str, value2);
    boost::this_fiber::yield();
    
    ++value2;
    logger.write("fiber {} incremented value2: {}", _id_str, value2);
    boost::this_fiber::yield();
    
    ++value2;
    logger.write("fiber {} incremented value2: {}", _id_str, value2);
    boost::this_fiber::yield();
    
    logger.write("current thread id of fn2: {}", w_thread::get_current_thread_id());
    
    logger.write("fiber {} waits for barrier", _id_str);
    b.wait();
    logger.write("fiber {} passed barrier", _id_str);
    
    ++value2;
    logger.write("fiber {} incremented value2: ", _id_str, value2);
    boost::this_fiber::yield();
    
    logger.write("fiber {} fn2 returns", _id_str);
}

void whatevah(char me)
{
    try
    {
        auto _my_thread = w_thread::get_current_thread_id();
        {
            logger.write("fiber {} started on thread {}", me, _my_thread);
        }
        for ( unsigned i = 0; i < 10; ++i)//loop ten times
        {
            boost::this_fiber::yield(); // yield to other fibers
            auto _new_thread = w_thread::get_current_thread_id();
            if ( _new_thread != _my_thread)
            {
                //test if fiber was migrated to another thread
                _my_thread = _new_thread;
				logger.write("fiber {} switched to thread {}", me, _my_thread);
            }
        }
    }
    catch (...)
    {
        logger.write("unhandled error happended");
    }
    std::unique_lock<std::mutex> _lk(mutex_count);
    if ( 0 == --fiber_count)
    {
        //Decrement fiber counter for each completed fiber
        _lk.unlock();
        cv_count.notify_all(); //Notify all fibers waiting on `cnd_count`
    }
}

void thread_func( boost::fibers::barrier* b)
{
	logger.write("thread started {}", w_thread::get_current_thread_id());
    boost::fibers::use_scheduling_algorithm< boost::fibers::algo::shared_work >();
    /*
        Install the scheduling algorithm `boost::fibers::algo::shared_work`
        in order to join the work sharing.
    */
    b->wait(); //sync with other threads: allow them to start processing
    std::unique_lock<std::mutex> _lk(mutex_count);
    cv_count.wait( _lk, []()
    {
        return 0 == fiber_count;
    });
    /*
        Suspend main fiber and resume worker fibers in the meanwhile.
        Main fiber gets resumed (e.g returns from `condition_variable_any::wait()`)
        if all worker fibers are complete.
    */
    assert(0 == fiber_count);
}

void simple_test()
{
	try
	{
		boost::fibers::fiber _f1(fn, "test", 5);

		ostringstream _buffer;
		_buffer << _f1.get_id();
		auto _id_str = _buffer.str();
		_buffer.clear();

		logger.write("fiber id (f1): {}", _id_str);
		_f1.join();
		logger.write("done");
	}
	catch (std::exception const& e)
	{
		logger.write("exception happended: {}", e.what());
	}
    catch (...)
    {
        logger.write("unhandled exception happended for simple test");
    }
}

void barrier_sync_test()
{
    //barrier sample
    try
    {
        boost::fibers::barrier _fb(2);
        boost::fibers::fiber _f1( &fn1, std::ref(_fb));
        boost::fibers::fiber _f2( &fn2, std::ref(_fb));
        _f1.join();
        _f2.join();
        
        logger.write("done");
    }
    catch ( std::exception const& e)
    {
        logger.write("exception happended for barrier test: {}", e.what());
    }
    catch (...)
    {
        logger.write("unhandled exception happended for barrier test");
    }
}

void work_sharing()
{
    logger.write("main thread started {}", w_thread::get_current_thread_id());
      
    boost::fibers::use_scheduling_algorithm< boost::fibers::algo::shared_work>();
    /*
        Install the scheduling algorithm `boost::fibers::algo::shared_work` in the main thread
        too, so each new fiber gets launched into the shared pool.
    */

    for ( char c : std::string("abcdefghijklmnopqrstuvwxyz"))
    {
        /*
            Launch a number of worker fibers; each worker fiber picks up a character
            that is passed as parameter to fiber-function `whatevah`.
            Each worker fiber gets detached.
        */
        boost::fibers::fiber([c]()
        {
            whatevah( c);
        }).detach();
        
        ++fiber_count; //Increment fiber counter for each new fiber
    }
    boost::fibers::barrier _b(4);//1(main thread) + 3(other threads)
    std::thread _threads[] =
    {
        //launch a couple of threads that join the work sharing
        std::thread( thread_func, &_b),
        std::thread( thread_func, &_b),
        std::thread( thread_func, &_b)
    };
    _b.wait(); //sync with other threads: allow them to start processing
    {
        std::unique_lock<std::mutex> _lk( mutex_count);
        cv_count.wait( _lk, []()
        {
            return 0 == fiber_count;
        } );
        /*
            Suspend main fiber and resume worker fibers in the meanwhile.
            Main fiber gets resumed (e.g returns from `condition_variable_any::wait()`)
            if all worker fibers are complete.
        */
    }
    /*
        Releasing lock of mtx_count is required before joining the threads, otherwise
        the other threads would be blocked inside condition_variable::wait() and
        would never return (deadlock).
    */
    assert( 0 == fiber_count);
    for ( auto& _t : _threads)
    {
        //wait for threads to terminate
        _t.join();
    }
}

WOLF_MAIN()
{
    w_logger_config _log_config;
    _log_config.app_name = L"20_fiber";
    _log_config.log_path = wolf::system::io::get_current_directoryW();
#ifdef __WIN32
    _log_config.log_to_std_out = false;
#else
    _log_config.log_to_std_out = true;
#endif
    //initialize logger, and log in to the output debug window of visual studio(just for windows) and Log folder inside running directory
    logger.initialize(_log_config);
    
    //log to output file
    logger.write(L"Wolf initialized");
    
    w_timer _timer;
    
    //run tests
    simple_test();
    barrier_sync_test();
    
    _timer.start();
    work_sharing();
    _timer.stop();
    
    logger.write("time is: {}", _timer.get_milliseconds());
    
    //release logger
    logger.release();

    return EXIT_SUCCESS;
}


