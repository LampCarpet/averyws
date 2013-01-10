#ifndef UTILITIES_CHUNK_VECTOR_ITERATOR
#define UTILITIES_CHUNK_VECTOR_ITERATOR

#include <iterator>
#include <cstdint>

namespace Utilities {

//forward declaration
template<class T,uint64_t N>
class ChunkVector;

template<class T,uint64_t N>
class ChunkVectorIterator : public std::iterator<std::random_access_iterator_tag, ChunkVector<T,N> > {
  private:
    uint64_t pos_;
    //not const because of operator[]
    //const in all other operations
    ChunkVector<T,N> &head_;
    T *p_;

  public:
  ChunkVectorIterator(ChunkVector<T,N> &head, uint64_t pos);
  ChunkVectorIterator(const ChunkVectorIterator<T,N>& it);

  ChunkVectorIterator<T,N>& operator++();
  ChunkVectorIterator<T,N> operator++(T);

  ChunkVectorIterator<T,N>& operator--();
  ChunkVectorIterator<T,N> operator--(T);

  T& operator[](const int index);
  bool operator==(const ChunkVectorIterator<T,N>& rhs);
  bool operator!=(const ChunkVectorIterator<T,N>& rhs);
  T& operator*();
};

template<class T, uint64_t N>
ChunkVectorIterator<T,N>::ChunkVectorIterator(ChunkVector<T,N> &head, uint64_t pos):
    p_(&head_[pos])
   ,pos_(pos)
   ,head_(head) {
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N>::ChunkVectorIterator(const ChunkVectorIterator<T,N>& it) :
    p_(it.p_)
   ,pos_(it.pos_)
   ,head_(it.head_){
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N>& ChunkVectorIterator<T,N>::operator++() {
    p_ = &head_[++pos_];
    return *this;
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N> ChunkVectorIterator<T,N>::operator++(T) {
    ChunkVectorIterator<T,N> tmp(*this);
    operator++();
    return tmp;
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N>& ChunkVectorIterator<T,N>::operator--() {
    p_ = &head_[--pos_];
    return *this;
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N> ChunkVectorIterator<T,N>::operator--(T) {
    ChunkVectorIterator<T,N> tmp(*this);
    operator--();
    return tmp;
}

template<class T, uint64_t N>
T &ChunkVectorIterator<T,N>::operator[](const int index) {
    return head_[index];
}

template<class T, uint64_t N>
bool ChunkVectorIterator<T,N>::operator==(const ChunkVectorIterator<T,N>& rhs) {
    return p_== rhs.p_;
}
template<class T, uint64_t N>
bool ChunkVectorIterator<T,N>::operator!=(const ChunkVectorIterator<T,N>& rhs) {
    return p_!=rhs.p_;
}

template<class T, uint64_t N>
T& ChunkVectorIterator<T,N>::operator*() {
    return *p_;
}

}
#endif
