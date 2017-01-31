/*
 
 */

#include <iostream>
#include <w_io.h>
#include <w_logger.h>
#include <w_game_time.h>

using namespace std;

int main(int argc, char** argv) 
{
    logger.initialize("test", "/home/pooyaeimandar/");
    logger.write("This is the road of victory");
    
    auto _is_file = wolf::system::io::get_is_file("/home/pooyaeimandar/client.ovpn");
    
//    wolf::system::w_game_time _game_time;
//    
//    for(size_t i = 0 ; i < 100000; ++i)
//    {
//        _game_time.tick([&]()
//        {
//            std::cout << "Time is " << _game_time.get_total_seconds() << '\n';
//        });
//    }
    
    logger.release();
    
    return 0;
}

