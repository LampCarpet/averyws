#ifndef UTILITIES_CHUNK_VECTOR
#define UTILITIES_CHUNK_VECTOR

#include <memory>
#include <vector>
#include <cstdint>

#include <utilities_chunk_vector_iterator.h>

namespace Utilities {


template<class T,uint64_t N>
class ChunkVector {
    public:
        ChunkVector();
        uint64_t size() const;

        T* last_chunk();
        T* new_chunk();
        uint64_t chunk_size() const;
        void close_last_chunk(uint64_t chunk_size);
        
        T& operator[] (const int index);
        ChunkVectorIterator<T,N> begin();
        const ChunkVectorIterator<T,N> cbegin() const;
        ChunkVectorIterator<T,N> end();
        const ChunkVectorIterator<T,N> cend() const;
    private:
        std::vector<std::unique_ptr<T[]> > chunk_vector_;
        uint64_t size_;
        uint64_t last_chunk_size_;
        bool open_;
};

template<class T,uint64_t N>
ChunkVector<T,N>::ChunkVector():
     size_(0)
    ,open_(true)
    ,last_chunk_size_(N){
    }

template<class T,uint64_t N>
T *ChunkVector<T,N>::last_chunk() {
    return &operator[](size_ - N);
}

template<class T,uint64_t N>
T *ChunkVector<T,N>::new_chunk() {
    if(open_) {
        chunk_vector_.push_back(std::unique_ptr<T[]>(new T[N]));
        size_ += N;
    }
    return last_chunk();
}

template<class T,uint64_t N>
uint64_t ChunkVector<T,N>::size() const{
    return size_;
}

template<class T,uint64_t N>
uint64_t ChunkVector<T,N>::chunk_size() const{
    return last_chunk_size_;
}

template<class T,uint64_t N>
void ChunkVector<T,N>::close_last_chunk(uint64_t chunk_size){
    last_chunk_size_ = chunk_size;
    size_ = static_cast<int64_t>( size_ - (N - chunk_size) );
    open_ = false;
}

template<class T,uint64_t N>
T& ChunkVector<T,N>::operator[] (const int index) {
    return chunk_vector_[index/N][index % N];
}

template<class T,uint64_t N>
ChunkVectorIterator<T,N> ChunkVector<T,N>::begin() {
    return ChunkVectorIterator<T,N>(*this,0);
}

template<class T,uint64_t N>
const ChunkVectorIterator<T,N> ChunkVector<T,N>::cbegin() const{
    return ChunkVectorIterator<T,N>(*this,0);
}

template<class T,uint64_t N>
ChunkVectorIterator<T,N> ChunkVector<T,N>::end() {
    return ChunkVectorIterator<T,N>(*this,size_);
}

template<class T,uint64_t N>
const ChunkVectorIterator<T,N> ChunkVector<T,N>::cend() const{
    return ChunkVectorIterator<T,N>(*this,size_);
}
}

#endif
