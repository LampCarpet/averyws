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
  public:
  ChunkVectorIterator(const ChunkVector<T,N> *head, uint64_t pos);
  ChunkVectorIterator(const ChunkVectorIterator<T,N>& it);

  ChunkVectorIterator<T,N>& operator++();
  ChunkVectorIterator<T,N> operator++(int placeholder);
  ChunkVectorIterator<T,N> operator+(int diff);
  ChunkVectorIterator<T,N> operator+(const ChunkVectorIterator<T,N> &rhs);

  ChunkVectorIterator<T,N>& operator--();
  ChunkVectorIterator<T,N> operator--(int placeholder);
  ChunkVectorIterator<T,N> operator-(int diff);
  ChunkVectorIterator<T,N> operator-(const ChunkVectorIterator<T,N> &rhs);
  
  ChunkVectorIterator<T,N>& operator=(const ChunkVectorIterator<T,N>& it);

  ChunkVectorIterator<T,N>& operator+=(int diff);
  ChunkVectorIterator<T,N>& operator-=(int diff);

  T& operator[](const int index);
  T& operator*();

  bool operator==(const ChunkVectorIterator<T,N>& rhs) const;
  bool operator!=(const ChunkVectorIterator<T,N>& rhs) const;

  bool operator<=(const ChunkVectorIterator<T,N> & rhs) const;
  bool operator< (const ChunkVectorIterator<T,N> & rhs) const;

  bool operator>=(const ChunkVectorIterator<T,N> & rhs) const;
  bool operator> (const ChunkVectorIterator<T,N> & rhs) const;
  
  private:
    uint64_t pos_;
    ChunkVector<T,N> *head_;
};

template<class T, uint64_t N>
ChunkVectorIterator<T,N>::ChunkVectorIterator(const ChunkVector<T,N> *head, uint64_t pos):
    pos_(pos)
   ,head_(const_cast<ChunkVector<T,N> *>(head)) {
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N>::ChunkVectorIterator(const ChunkVectorIterator<T,N>& it) :
    pos_(it.pos_)
   ,head_(it.head_){
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N>& ChunkVectorIterator<T,N>::operator++() {
    ++pos_;
    return *this;
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N> ChunkVectorIterator<T,N>::operator++(int placeholder) {
    ChunkVectorIterator<T,N> tmp(*this);
    ++pos_;
    return tmp;
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N> ChunkVectorIterator<T,N>::operator+(int diff) {
    ChunkVectorIterator<T,N> tmp(*this);
    tmp.pos_ += diff;
    return tmp;
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N> ChunkVectorIterator<T,N>::operator+(const ChunkVectorIterator<T,N> &rhs) {
    ChunkVectorIterator<T,N> tmp(*this);
    tmp.pos_ += rhs.pos_;
    return tmp;
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N>& ChunkVectorIterator<T,N>::operator--() {
    --pos_;
    return *this;
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N> ChunkVectorIterator<T,N>::operator--(int placeholder) {
    ChunkVectorIterator<T,N> tmp(*this);
    --pos_;
    return tmp;
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N> ChunkVectorIterator<T,N>::operator-(int diff) {
    ChunkVectorIterator<T,N> tmp(*this);
    tmp.pos_ -= diff;
    return tmp;
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N> ChunkVectorIterator<T,N>::operator-(const ChunkVectorIterator<T,N> &rhs) {
    ChunkVectorIterator<T,N> tmp(*this);
    tmp.pos_ -= rhs.pos_;
    return tmp;
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N>& ChunkVectorIterator<T,N>::operator=(const ChunkVectorIterator<T,N>& it) {
    head_ = it.head_;
    pos_  = it.pos_;
    return *this;
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N>& ChunkVectorIterator<T,N>::operator+=(int diff) {
    pos_+= diff;
    return *this;
}

template<class T, uint64_t N>
ChunkVectorIterator<T,N>& ChunkVectorIterator<T,N>::operator-=(int diff) {
    pos_-= diff;
    return *this;
}

template<class T, uint64_t N>
T &ChunkVectorIterator<T,N>::operator[](const int index) {
    return (*head_)[pos_ + index];
}

template<class T, uint64_t N>
T& ChunkVectorIterator<T,N>::operator*() {
    return (*head_)[pos_];
}

template<class T, uint64_t N>
bool ChunkVectorIterator<T,N>::operator==(const ChunkVectorIterator<T,N>& rhs) const {
    return pos_ == rhs.pos_;
}
template<class T, uint64_t N>
bool ChunkVectorIterator<T,N>::operator!=(const ChunkVectorIterator<T,N>& rhs) const{
    return pos_ != rhs.pos_;
}

template<class T, uint64_t N>
bool ChunkVectorIterator<T,N>::operator>=(const ChunkVectorIterator<T,N>& rhs) const{
    return pos_ >= rhs.pos_;
}

template<class T, uint64_t N>
bool ChunkVectorIterator<T,N>::operator>(const ChunkVectorIterator<T,N>& rhs) const{
    return pos_ > rhs.pos_;
}

template<class T, uint64_t N>
bool ChunkVectorIterator<T,N>::operator<=(const ChunkVectorIterator<T,N>& rhs) const{
    return pos_ <= rhs.pos_;
}

template<class T, uint64_t N>
bool ChunkVectorIterator<T,N>::operator<(const ChunkVectorIterator<T,N>& rhs) const{
    return pos_ < rhs.pos_;
}

}
#endif
