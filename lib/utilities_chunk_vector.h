#ifndef UTILITIES_CHUNK_VECTOR
#define UTILITIES_CHUNK_VECTOR

#include <memory>
#include <vector>
#include <cstdint>

#include <utilities_chunk_vector_iterator.h>

//debug#include <iostream>

namespace Utilities {


template<class T,uint64_t N>
class ChunkVector {
    public:
        typedef ChunkVectorIterator<T,N> iterator;
        //TODO, make const_iterator work with:
        //typedef const ChunkVectorIterator<T,N> const_iterator;
        typedef ChunkVectorIterator<T,N> const_iterator;

        ChunkVector();
        uint64_t size() const;

        T* last_chunk();
        T* new_chunk();
        uint64_t chunk_size() const;
        void close_last_chunk(const uint64_t chunk_size);
        const_iterator end_of_chunk(const_iterator &it) const;
        
        T& operator[] (const int index);
        const T& operator[] (const int index) const;

        iterator begin();
        const_iterator begin() const;
        const_iterator cbegin() const;

        iterator end();
        const_iterator end() const;
        const_iterator cend() const;
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
void ChunkVector<T,N>::close_last_chunk(const uint64_t chunk_size){
    last_chunk_size_ = chunk_size;
    size_ = static_cast<int64_t>( size_ - (N - chunk_size) );
    open_ = false;
}

template<class T,uint64_t N>
typename ChunkVector<T,N>::const_iterator ChunkVector<T,N>::ChunkVector<T,N>::end_of_chunk(typename ChunkVector<T,N>::const_iterator &it) const{
    uint64_t pos = N*(it.pos()/N + 1);
    if( pos > size_ ) {
       pos = size_; 
    }
    return ChunkVector<T,N>::const_iterator(this, pos);
}

template<class T,uint64_t N>
T& ChunkVector<T,N>::operator[] (const int index) {
    //debugstd::cout << "requesting index:" << index << "chunk size: " << N << "vec size: " << chunk_vector_.size() << "pos: " << index/N << "," << index % N << std::endl;
    return chunk_vector_[index/N][index % N];
}

template<class T,uint64_t N>
const T& ChunkVector<T,N>::operator[] (const int index) const{
    //debugstd::cout << "requesting const index:" << index << "chunk size: " << N << "vec size: " << chunk_vector_.size() << "pos: " << index/N << "," << index % N << std::endl;
    return chunk_vector_[index/N][index % N];
}

template<class T,uint64_t N>
typename ChunkVector<T,N>::iterator ChunkVector<T,N>::begin() {
    return ChunkVectorIterator<T,N>(this,0);
}

template<class T,uint64_t N>
typename ChunkVector<T,N>::const_iterator ChunkVector<T,N>::begin() const{
    return ChunkVectorIterator<T,N>(this,0);
}

template<class T,uint64_t N>
typename ChunkVector<T,N>::const_iterator ChunkVector<T,N>::cbegin() const{
    return ChunkVectorIterator<T,N>(this,0);
}

template<class T,uint64_t N>
typename ChunkVector<T,N>::iterator ChunkVector<T,N>::end() {
    return ChunkVectorIterator<T,N>(this,size_);
}

template<class T,uint64_t N>
typename ChunkVector<T,N>::const_iterator ChunkVector<T,N>::end() const{
    return ChunkVectorIterator<T,N>(this,size_);
}

template<class T,uint64_t N>
typename ChunkVector<T,N>::const_iterator ChunkVector<T,N>::cend() const{
    return ChunkVectorIterator<T,N>(this,size_);
}

}

#endif
