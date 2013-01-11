#include <ws_frame.h>
#include <utilities_chunk_vector.h>
#include <iostream>

int main(){
    using Websocket::Frame;
    using Utilities::ChunkVector;

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

    return 0;
}
