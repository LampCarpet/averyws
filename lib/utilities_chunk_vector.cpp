#include <utilities_chunk_vector.hpp>
#include <stdexcept>

namespace Utilities {
ChunkVector::ChunkVector(uint64_t chunk_size):
    open_(true)
    ,chunk_size_(chunk_size) {
    }

uint8_t& ChunkVector::last_chunk() {
    return chunk_vector_.back()->at(0);
}

uint8_t &ChunkVector::new_chunk() {
    if(!open_) throw std::logic_error("ChunkVector closed but new chunk was requested."); 
    
    chunk_vector_.push_back(std::make_shared<chunk_t >(chunk_size_));
    return last_chunk();
}

uint64_t ChunkVector::size() const{
    if(chunk_vector_.size() == 0) return 0;
    return ( chunk_vector_.size() -1 ) * chunk_size_ + chunk_vector_.back()->size();
}

uint64_t ChunkVector::chunk_size() const{
    return chunk_size_;
}

void ChunkVector::close_last_chunk(const uint64_t chunk_size){
    chunk_vector_.back()->resize(chunk_size);
    open_ = false;
}

uint8_t& ChunkVector::at(uint64_t i) {
    return chunk_vector_[i/chunk_size_]->at(i%chunk_size_);
}

vector_chunks_t::const_iterator ChunkVector::chunk_cbegin() const{
    return chunk_vector_.cbegin();
}

vector_chunks_t::const_iterator ChunkVector::chunk_cend() const{
    return chunk_vector_.cend();
}

vector_chunks_t::iterator ChunkVector::chunk_begin(){
    return chunk_vector_.begin();
}

vector_chunks_t::iterator ChunkVector::chunk_end(){
    return chunk_vector_.end();
}

chunk_sp& ChunkVector::chunk_back(){
    return chunk_vector_.back();
}
}
