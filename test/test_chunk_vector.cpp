#include <utilities_chunk_vector.h>
#include <iostream>
#include <string>

#include <ios>

#include <cstring>
#include <vector>
#include <array>
#include <algorithm>

int main(){

    using Utilities::ChunkVector;
    ChunkVector<int,5> chunks;
    auto int_arr = chunks.new_chunk();
    std::cout << "chunk size " << chunks.chunk_size() << std::endl;
    for(int i = 0; i < chunks.chunk_size(); ++i) {
       int_arr[i] = i; 
    }
    int_arr = chunks.new_chunk();
    for(int i = 0; i < chunks.chunk_size(); ++i) {
       int_arr[i] = i+10; 
    }
    int_arr = chunks.new_chunk();
    for(int i = 0; i < 2; ++i) {
       int_arr[i] = i+20; 
    }
    chunks.close_last_chunk(2);

    for( auto i = chunks.begin(); i != chunks.end(); ++i)
    {
        std::cout << *i << std::endl;
    }


}
