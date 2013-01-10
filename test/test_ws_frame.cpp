#include <ws_frame.h>
#include <iostream>
#include <string>

#include <ios>

#include <cstring>
#include <vector>
#include <array>
#include <algorithm>

int main(){

    using Websocket::Frame;

    {
    std::array<unsigned char,11> hello;

    hello[0]  = 0x81;
    hello[1]  = 0x85;
    hello[2]  = 0x37; 
    hello[3]  = 0xfa; 
    hello[4]  = 0x21; 
    hello[5]  = 0x3d; 
    hello[6]  = 0x7f; 
    hello[7]  = 0x9f; 
    hello[8]  = 0x4d; 
    hello[9]  = 0x51; 
    hello[10] = 0x58;


    Frame<std::array<unsigned char,11> > frame(hello);
    std::cout << 
        std::string(reinterpret_cast<const char *>(frame.data()) 
                ,frame.length() )
              << std::endl;

    }

    std::cout << "----" << std::endl;
    {
    std::string hello_str = "hi";
    hello_str.resize(20,'@'); 
    *(hello_str.end()-1) = 'E';

    std::vector<unsigned char> hello2;
    uint64_t size = Frame<std::vector<unsigned char> >::reserve(hello_str.size(),0x8180);
    hello2.reserve(size+hello_str.size());

    hello2.insert(hello2.begin(),size,'$');
    hello2.insert(hello2.begin()+size,hello_str.begin(),hello_str.end());

    Frame<std::vector<unsigned char> > frame3(hello2,size,hello_str.size(),0x8180);
    Frame<std::vector<unsigned char> > frame4(hello2);

    std::cout << frame3.length() << " " << frame4.length() << std::endl;
    std::cout << std::string(frame4.data(),frame4.data() + frame4.length()) << std::endl;
    }
    
    
    std::cout << "----" << std::endl;
    {
    //the final output will be hi@@@, but we will fill the buffer in 2 steps
    std::string data = "hihi@@@@BBBBbibiEEEE####";

    std::vector<unsigned char> buffer;
    uint64_t size = Frame<std::vector<unsigned char> >::reserve(data.size(),0x8180);
    buffer.reserve(size+data.size());
    buffer.insert(buffer.begin(),size,'$');
    buffer.insert(buffer.begin()+size,data.begin(),data.end());

    Frame<std::vector<unsigned char> > frame(buffer,size,data.size(),0x8180);

    std::vector<unsigned char> buffer2;
    buffer2.reserve(buffer.size() - 9);
    buffer2.insert(buffer2.begin(),buffer.begin(),buffer.end() - 9);
    
    Frame<std::vector<unsigned char> > frame2(buffer2);
    std::cout << "consumed: " << frame2.consumed() << " remaining: " << frame2.remaining() << std::endl;
    std::vector<unsigned char> buffer3;
    buffer3.reserve(buffer.size());
    buffer3.insert(buffer3.begin(),buffer2.begin(),buffer2.end());
    buffer3.insert(buffer3.end(),buffer.end()-9,buffer.end());

    frame2.update_buffer(buffer3);
    frame2.consume();
    std::cout << "consumed: " << frame2.consumed() << " remaining: " << frame2.remaining() << std::endl;


    std::cout << "lengths: " << frame.length() << " " << frame2.length() << std::endl;
    std::cout << std::string(frame2.data(),frame2.data() + frame2.length()) << std::endl;
    }

    //std::cout << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(hello2[0])) << std::endl;
    //std::cout << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(hello2[1])) << std::endl;
    //std::cout << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(hello2[2])) << std::endl;
    //std::cout << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(hello2[3])) << std::endl;
    //std::cout << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(hello2[4])) << std::endl;
    //std::cout << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(hello2[5])) << std::endl;
    //std::cout << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(hello2[6])) << std::endl;
    //std::cout << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(hello2[7])) << std::endl;
    //std::cout << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(hello2[8])) << std::endl;
    //std::cout << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(hello2[9])) << std::endl;
    //std::cout << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(hello2[10])) << std::endl;
    return 0;
}
