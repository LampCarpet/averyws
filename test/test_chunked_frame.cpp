#include <ws_frame.h>
#include <utilities_chunk_vector.h>
#include <iostream>

int main(){
    using Websocket::Frame;
    using Utilities::ChunkVector;
    
    {
    ChunkVector<unsigned char,11> buffer;
    auto char_array  = buffer.new_chunk();

    char_array[0]  = 0x81;
    char_array[1]  = 0x85;
    char_array[2]  = 0x37; 
    char_array[3]  = 0xfa; 
    char_array[4]  = 0x21; 
    char_array[5]  = 0x3d; 
    char_array[6]  = 0x7f; 
    char_array[7]  = 0x9f; 
    char_array[8]  = 0x4d; 
    char_array[9]  = 0x51; 
    char_array[10] = 0x58;

    Frame<ChunkVector<unsigned char,11> > frame(buffer);
    std::cout << frame.length() << " " <<
        std::string(reinterpret_cast<char *>(&(*frame.data())) ,frame.length() ) << std::endl;
    }
    
    std::cout << "----" << std::endl;

    {
    std::string hello_str = "hi";
    hello_str.resize(20,'@'); 
    *(hello_str.end()-1) = 'E';

    ChunkVector<unsigned char,9> buffer;
    uint64_t size = Frame<ChunkVector<unsigned char,9> >::reserve(hello_str.size(),0x8180);
    while(buffer.size() < size + hello_str.size()) {
        buffer.new_chunk();
    }

    //.insert would be useful in ChunkVector, would translate the 2 for loops to :
    //buffer.insert(buffer.begin(),size,'$');
    //buffer.insert(buffer.begin()+size,hello_str.begin(),hello_str.end());

    for(int i = 0; i < size; ++i) {
        buffer[i] = '$';
    }

    auto bit = buffer.begin() + size;
    for(auto it = hello_str.begin(); it != hello_str.end(); ++bit, ++it) {
        *bit = *it;
    }

    buffer.close_last_chunk(( size + hello_str.size() ) % buffer.chunk_size());
    std::cout << "buffer size" << buffer.size() << " expected " << size +  hello_str.size() << std::endl;
    

    Frame<ChunkVector<unsigned char,9> > frame3(buffer,size,hello_str.size(),0x8180);
    Frame<ChunkVector<unsigned char,9> > frame4(buffer);
    std::cout << "consumed: " << frame4.consumed() << " remaining: " << frame4.remaining() << std::endl;

    for(auto it = frame4.data(); it < frame4.data() + frame4.length(); it = buffer.end_of_chunk(it)) {
        std::cout << std::string(reinterpret_cast<char *>(&*it),buffer.end_of_chunk(it).pos() - it.pos());
    }

    std::cout << std::endl;

    }

    std::cout << "----" << std::endl;

    {
    //the final output will be hi@@@, but we will fill the buffer in 2 steps
    std::string data = "hihi@@@@BBBBbibiEEEE####";
    
    std::vector<unsigned char> socket_buffer;
    uint64_t size = Frame<std::vector<unsigned char> >::reserve(data.size(),0x8180);
    socket_buffer.reserve(size+data.size());
    socket_buffer.insert(socket_buffer.begin(),size,'$');
    socket_buffer.insert(socket_buffer.begin()+size,data.begin(),data.end());
    Frame<std::vector<unsigned char> > frame(socket_buffer,size,data.size(),0x8180);

    auto socket_it = socket_buffer.begin();

    ChunkVector<unsigned char,9> buffer;
    auto chunk = buffer.new_chunk();
    for(auto it = chunk; it != chunk + buffer.chunk_size() && socket_it != socket_buffer.end(); ++it,++socket_it) {
        *it = *socket_it;
    }

    Frame<ChunkVector<unsigned char,9> > frame2(buffer);

    while(socket_it != socket_buffer.end()) {
        //request memory
        auto chunk = buffer.new_chunk();
        std::cout << "new buffer, now at size:" << buffer.size() << std::endl;
        //tell the 'socket' to write N bytes (N defined in template) to the allocated chunk of contiguous memeory
        //this loop imitates socket data being copied over to the chunk. In reality this data transfer is executed by ASIO 
        for(auto it = chunk; it != chunk + buffer.chunk_size() && socket_it != socket_buffer.end(); ++it,++socket_it) {
            *it = *socket_it;
        }
        frame2.consume();
    }

    //we may have over-allocated a little so we close the chunk where the data ends
    buffer.close_last_chunk(( socket_buffer.size() ) % buffer.chunk_size());

    std::cout << "lengths: " << frame.length() << " " << frame2.length() << std::endl;
    for(auto it = frame2.data(); it < frame2.data() + frame2.length(); it = buffer.end_of_chunk(it)) {
        std::cout << std::string(reinterpret_cast<char *>(&*it),buffer.end_of_chunk(it).pos() - it.pos());
    }
    std::cout << std::endl;

    }
    
    

    return 0;
}
