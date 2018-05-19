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
        logger.write(std::to_string(i) + " : " + pStr);
        boost::this_fiber::yield();
    }
}

inline void fn1( boost::fibers::barrier& b)
{
    auto _id = boost::this_fiber::get_id();
    logger.write("fiber " + std::to_string(_id)  + ": fn1 entered");
    
    ++value1;
    logger.write("fiber " + std::to_string(_id)  + ": incremented value1:" + std::to_string(value1));
    boost::this_fiber::yield();
    
    logger.write("current thread id of fn1: " + std::to_string(w_thread::get_current_thread_id()));
    
    logger.write("fiber " + std::to_string(_id)  + ": waits for barrier");
    b.wait();
    logger.write("fiber " + std::to_string(_id)  + ": passed barrier");
    
    ++value1;
    logger.write("fiber " + std::to_string(_id)  + ": incremented value1:" + std::to_string(value1));
    boost::this_fiber::yield();
    
    ++value1;
    logger.write("fiber " + std::to_string(_id)  + ": incremented value1:" + std::to_string(value1));
    boost::this_fiber::yield();
    
    ++value1;
    logger.write("fiber " + std::to_string(_id)  + ": incremented value1:" + std::to_string(value1));
    boost::this_fiber::yield();
    
    logger.write("fiber " + std::to_string(_id)  + ": fn1 returns");
}

inline void fn2( boost::fibers::barrier & b)
{
    auto _id = boost::this_fiber::get_id();
    logger.write("fiber " + std::to_string(_id)  + ": fn2 entered");
    
    ++value2;
    logger.write("fiber " + std::to_string(_id)  + ": incremented value2:" + std::to_string(value2));
    boost::this_fiber::yield();
    
    ++value2;
    logger.write("fiber " + std::to_string(_id)  + ": incremented value2:" + std::to_string(value2));
    boost::this_fiber::yield();
    
    ++value2;
    logger.write("fiber " + std::to_string(_id)  + ": incremented value2:" + std::to_string(value2));
    boost::this_fiber::yield();
    
    logger.write("current thread id of fn2: " + std::to_string(w_thread::get_current_thread_id()));
    
    logger.write("fiber " + std::to_string(_id)  + ": waits for barrier");
    b.wait();
    logger.write("fiber " + std::to_string(_id)  + ": passed barrier");
    
    ++value2;
    logger.write("fiber " + std::to_string(_id)  + ": incremented value2:" + std::to_string(value2));
    boost::this_fiber::yield();
    
    logger.write("fiber " + std::to_string(_id)  + ": fn2 returns");
}

void whatevah(char me)
{
    try
    {
        std::thread::id my_thread = std::this_thread::get_id(); /*< get ID of initial thread >*/
        {
            std::ostringstream buffer;
            buffer << "fiber " << me << " started on thread " << my_thread << '\n';
            std::cout << buffer.str() << std::flush;
        }
        for ( unsigned i = 0; i < 10; ++i)/*< loop ten times >*/
        {
            
            boost::this_fiber::yield(); /*< yield to other fibers >*/
            std::thread::id new_thread = std::this_thread::get_id(); /*< get ID of current thread >*/
            if ( new_thread != my_thread) { /*< test if fiber was migrated to another thread >*/
                my_thread = new_thread;
                std::ostringstream buffer;
                buffer << "fiber " << me << " switched to thread " << my_thread << '\n';
                std::cout << buffer.str() << std::flush;
            }
        }
    } catch ( ... ) {
    }
    std::unique_lock<std::mutex> lk(mutex_count);
    if ( 0 == --fiber_count)
    { /*< Decrement fiber counter for each completed fiber. >*/
        lk.unlock();
        cv_count.notify_all(); /*< Notify all fibers waiting on `cnd_count`. >*/
    }
}

void thread( boost::fibers::barrier* b)
{
    std::ostringstream buffer;
    buffer << "thread started " << std::this_thread::get_id() << std::endl;
    std::cout << buffer.str() << std::flush;
    boost::fibers::use_scheduling_algorithm< boost::fibers::algo::shared_work >(); /*<
                                                                                    Install the scheduling algorithm `boost::fibers::algo::shared_work` in order to
                                                                                    join the work sharing.
                                                                                    >*/
    b->wait(); /*< sync with other threads: allow them to start processing >*/
    std::unique_lock<std::mutex> lk( mutex_count);
    cv_count.wait( lk, [](){ return 0 == fiber_count; } ); /*<
                                                             Suspend main fiber and resume worker fibers in the meanwhile.
                                                             Main fiber gets resumed (e.g returns from `condition_variable_any::wait()`)
                                                             if all worker fibers are complete.
                                                             >*/
    BOOST_ASSERT( 0 == fiber_count);
}

void simple_test()
{
    try
    {
        boost::fibers::fiber _f1( fn, "test", 5);
        logger.write("fiber id (f1): " + std::to_string(_f1.get_id()));
        _f1.join();
        logger.write("done");
    }
    catch ( std::exception const& e)
    {
        logger.write("exception happended: " + std::string(e.what()));
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
        boost::fibers::fiber _f1( & fn1, std::ref( _fb) );
        boost::fibers::fiber _f2( & fn2, std::ref( _fb) );
        _f1.join();
        _f2.join();
        
        logger.write("done");
    }
    catch ( std::exception const& e)
    {
        logger.write("exception happended for barrier test: " + std::string(e.what()));
    }
    catch (...)
    {
        logger.write("unhandled exception happended for barrier test");
    }
}

void work_sharing()
{
    
}

WOLF_MAIN()
{
    //initialize logger, and log in to the output debug window of visual studio(just for windows) and Log folder inside running directory
    logger.initialize(L"20_fiber", wolf::system::io::get_current_directoryW());
    
    //log to output file
    logger.write(L"Wolf initialized");
    
    //run tests
    simple_test();
    barrier_sync_test();
    work_sharing();
    
    //output a message to the log file
    logger.write(L"shutting down Wolf");

    //release logger
    logger.release();

    return EXIT_SUCCESS;
}


