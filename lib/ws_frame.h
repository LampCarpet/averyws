//ws_frame.h
//
//
// 0                   1                   2                   3           
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1    Dec|Hex|Oct|Bin
//+-+-+-+-+-------+-+-------------+-------------------------------+   ---|---|---|----
//|F|R|R|R| opcode|M| Payload len |    Extended payload length    |   0  |0  |00 |0000
//|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |   1  |1  |01 |0001
//|N|V|V|V|       |S|             |   (if payload len==126/127)   |   2  |2  |02 |0010
//| |1|2|3|       |K|             |                               |   3  |3  |03 |0011
//+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +   4  |4  |04 |0100
//|     Extended payload length continued, if payload len == 127  |   5  |5  |05 |0101
//+ - - - - - - - - - - - - - - - +-------------------------------+   6  |6  |06 |0110
//|                               |Masking-key, if MASK set to 1  |   7  |7  |07 |0111
//+-------------------------------+-------------------------------+   8  |8  |10 |1000
//| Masking-key (continued)       |          Payload Data         |   9  |9  |11 |1001
//+-------------------------------- - - - - - - - - - - - - - - - +   10 |A  |12 |1010
//:                     Payload Data continued ...                :   11 |B  |13 |1011
//+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +   12 |C  |14 |1100
//|                     Payload Data continued ...                |   13 |D  |15 |1101
//+---------------------------------------------------------------+   14 |E  |16 |1110
//                                                                    15 |F  |17 |1111

#ifndef WS_FRAME
#define WS_FRAME

#include <cstdint>

#include <stdexcept>
#include <utilities_random.h>

//debug#include <iostream>

namespace Websocket {

template<class T>
class Frame {
  public:
    typedef typename T::const_iterator cit_t;
    typedef typename T::iterator it_t;

    Frame(T &buffer,uint64_t frame_offset = 0);
    Frame(T &buffer,uint64_t reserved_header_length, uint64_t data_length, uint16_t flags);
    
    uint64_t length();
    uint64_t length() const;
   
    bool fin() const;
    uint64_t consumed();
    bool headerConsume();
    bool header_consumed_;
    
    void update_buffer(T &buffer);
    uint64_t consume();
    uint64_t remaining();

    it_t data();
    cit_t data() const;

    static uint64_t reserve(const uint64_t max_length,const uint16_t flags);
    

  private:
    T &buffer_;

    uint64_t data_length_;
    uint8_t data_length_byte_size_;

    uint64_t frame_offset_;
    uint8_t data_offset_;
    uint8_t mask_offset_;
    
    uint64_t data_consumed_;
    uint64_t header_size_required_;

    void process_header();
    void flags2frame(uint16_t flags);
    void length2frame(uint64_t data_length);
};

template<class T>
void Frame<T>::process_header() {

}

template<class T>
typename Frame<T>::it_t Frame<T>::data() { return buffer_.begin() + data_offset_;}

template<class T>
typename Frame<T>::cit_t Frame<T>::data() const { return buffer_.cbegin() + data_offset_ ;}

template<class T>
uint64_t Frame<T>::consumed() { return data_consumed_;}

template<class T>
uint64_t Frame<T>::remaining() { return data_length_ - data_consumed_;}

template<class T>
bool Frame<T>::fin() const { return buffer_[frame_offset_] & 0x80;}



template<class T>
uint64_t Frame<T>::length() { return data_length_;}

template<class T>
uint64_t Frame<T>::length() const { return data_length_;}

template<class T>
void Frame<T>::update_buffer(T &buffer) {
    buffer_ = buffer;
}

template<class T>
Frame<T>::Frame(T &buffer,uint64_t frame_offset):
     buffer_(buffer)
    ,frame_offset_(frame_offset)
    ,data_consumed_(0)
    ,header_size_required_(2 + frame_offset)
    ,header_consumed_(false)
    ,data_offset_(0){

    if( frame_offset > buffer.size()) {
        throw std::length_error("frame offset is larger than the buffer size");
    }
    cit_t frame = buffer_.cbegin() + frame_offset_;
    consume();
}

template<class T>
bool Frame<T>::headerConsume() {
    if(header_consumed_) {
        //debugstd::cout << "header has been read before" << std::endl;
        return true;
    }
    
    if (buffer_.size() < header_size_required_) {
        //debugstd::cout << "header has not completely been read" << std::endl;
        return false;
    }

    cit_t frame = buffer_.cbegin() + frame_offset_;
    data_length_ = frame[1] & 0x7F;
    if(data_length_ <= 125) {
        data_length_byte_size_ = 1;
    } else if(data_length_ == 126) {
        data_length_byte_size_ = 3;
        header_size_required_ +=2;
    } else if (data_length_ == 127) {
        data_length_byte_size_ = 9;
        header_size_required_ += 8;
    }

    if(buffer_.size() < header_size_required_) {
        return false;
    }

    if(data_length_ == 126) {
        data_length_ = (static_cast<uint64_t>(frame[2]) << 8) 
                     | (static_cast<uint64_t>(frame[3]) << 0);
    } else if (data_length_ == 127) {
        data_length_ = (static_cast<uint64_t>(frame[2]) << 56) 
                     | (static_cast<uint64_t>(frame[3]) << 48)
                     | (static_cast<uint64_t>(frame[4]) << 40)
                     | (static_cast<uint64_t>(frame[5]) << 32)
                     | (static_cast<uint64_t>(frame[6]) << 24)
                     | (static_cast<uint64_t>(frame[7]) << 16)
                     | (static_cast<uint64_t>(frame[8]) <<  8) 
                     | (static_cast<uint64_t>(frame[9]) <<  0);
    }
    if(data_offset_ == 0) {
        if(frame[1] & 0x80) {
            mask_offset_ = frame_offset_ + 1 + data_length_byte_size_;
            data_offset_ = frame_offset_ + 1 + data_length_byte_size_ + 4;
            header_size_required_ += 4;
        } else {
            data_offset_ = frame_offset_ + 1 + data_length_byte_size_;
        }
    }
    header_consumed_ = buffer_.size() >= header_size_required_;
    //debugstd::cout << "header consumed? " << header_consumed_ << std::endl;
    return header_consumed_;
}

//before:
//|---buffer----------------|---data---|
//after, best-case:
//|---header----------------|---data---|
//after, worst-case:
//|---buffer---|---header---|---data---|
//
//if you use the reserve function the most you can lose is 8 bytes if you overestimate
//If your prediction is accurate you will lose 0 bytes and buffer will lign-up with header.
//if you underestimate and there isn't enough room in the buffer, a length_error exception will be thrown
//Byte loss chart:
//  ______________________________________________________________
// |Actual/Estimate  | 1-125 bytes | 126-65535 | 65536-2^64 bytes |
// |_________________|_____________|___________|__________________|
// |1-125 bytes      |     0       |    ERR    |       ERR        |
// |_________________|_____________|___________|__________________|
// |126-65535        |     2       |     0     |       ERR        |
// |_________________|_____________|___________|__________________|
// |65536-2^64 bytes |     8       |     6     |        0         |
// |_________________|_____________|___________|__________________|
//

template<class T>
Frame<T>::Frame(T &buffer,uint64_t reserved_header_length, uint64_t data_length, uint16_t flags):
     buffer_(buffer)
    ,data_consumed_(0)
{
    header_size_required_ = reserve(data_length,flags);
    if(reserved_header_length < header_size_required_) {
        throw std::length_error("WebSocket header is larger than what was allocated");
    }
    frame_offset_ = reserved_header_length - header_size_required_;
    //debugstd::cout << "frame offset " << frame_offset_ << std::endl;
    cit_t frame = buffer_.cbegin() + frame_offset_;
    flags2frame(flags);
    length2frame(data_length);
    if(frame[1] & 0x80) {
        mask_offset_ = frame_offset_ + 1 + data_length_byte_size_;
        data_offset_ = frame_offset_ + 1 + 4 + data_length_byte_size_;
        it_t mask = buffer_.begin() + mask_offset_;
        
        std::vector<unsigned uint8_t> random_bytes = Utilities::Random::bytes_safe(4);
        //debugstd::cout << "mask: " << random_bytes[0] << " " << random_bytes[1] << " " << random_bytes[2] << " " << random_bytes[3] << std::endl;
        mask[0] = static_cast<uint8_t>(random_bytes[0]);
        mask[1] = static_cast<uint8_t>(random_bytes[1]);
        mask[2] = static_cast<uint8_t>(random_bytes[2]);
        mask[3] = static_cast<uint8_t>(random_bytes[3]);
        consume();
    } else {
        data_offset_ = 1 + data_length_byte_size_;
    }
}
    
template<class T>
uint64_t Frame<T>::reserve(const uint64_t length , const uint16_t flags) {
    uint64_t base_required = 2;
    if(flags & 0x0080) base_required += 4;
    if(length > 125) {
        if(length > 65535) {
            base_required += 8;
        } else {
            base_required += 2;
        }
    }
    return base_required;
}


template<class T>
void Frame<T>::length2frame(uint64_t length){
    data_length_ = length;

    it_t frame = buffer_.begin() + frame_offset_;
    if(data_length_ <= 125) {
        frame[1] = data_length_ | (frame[1] & 0x80); 
        data_length_byte_size_ = 1;
    } else if (data_length_ < 65535) {
        frame[1] = 126 | (frame[1] & 0x80);
        frame[2] = (data_length_ & 0x000000000000ff00) >>  8;
        frame[3] = (data_length_ & 0x00000000000000ff) >>  0;
        data_length_byte_size_ = 3;
    } else {
        frame[1] = 127 | (frame[1] & 0x80);
        //read 8 bytes
        frame[2] = (data_length_ & 0xff00000000000000) >> 56;
        frame[3] = (data_length_ & 0x00ff000000000000) >> 48;
        frame[4] = (data_length_ & 0x0000ff0000000000) >> 40;
        frame[5] = (data_length_ & 0x000000ff00000000) >> 32;
        
        frame[6] = (data_length_ & 0x00000000ff000000) >> 24;
        frame[7] = (data_length_ & 0x0000000000ff0000) >> 16;
        frame[8] = (data_length_ & 0x000000000000ff00) >>  8;
        frame[9] = (data_length_ & 0x00000000000000ff) >>  0;

        data_length_byte_size_ = 9;
    }
}

template<class T>
uint64_t Frame<T>::consume() {
    if(!headerConsume()) return 0;
    it_t mask = buffer_.begin() + mask_offset_;
    it_t data = buffer_.begin() + data_offset_;

    it_t it = data + data_consumed_;

    //unmask 4 bytes at a time
    uint64_t consume = data_length_ - data_consumed_;
    cit_t end = data + data_length_;
    //debugstd::cout << "consuming " << consume << std::endl;
    if( buffer_.size() < data_length_ + data_offset_ ){
        //buffer doesn't contain all the data so we will read as much as we can
        //debugstd::cout << buffer_.size() << " " << data_offset_ << " " << data_consumed_ << std::endl;
        consume = static_cast<int64_t>(buffer_.size() - data_offset_ - data_consumed_);
        end = buffer_.cend();
        //debugstd::cout << "actually, buffer filled, consuming " << consume << std::endl;

    }

    if(consume == 0){ return 0; }

    if( mask[0] == 0x00 && mask[1] == 0x00 && mask[2] == 0x00 && mask[3] == 0x00) {
        //debugstd::cout << "bailing, mask unset" <<  std::endl;
        data_consumed_ += consume;
        return data_consumed_;
    }

    if(consume >= 4) {
        //debugstd::cout << "consuming more than 4 bytes: " << consume << std::endl;
        for( cit_t end_4 = end-4 ; it <= end_4; it+=4) {
            //debugstd::cout << "consumed 4 bytes " << *it << *(it+1)<< *(it+2)<< *(it+3);
            it[0] ^= mask[(data_consumed_ + 0) % 4];
            it[1] ^= mask[(data_consumed_ + 1) % 4];
            it[2] ^= mask[(data_consumed_ + 2) % 4];
            it[3] ^= mask[(data_consumed_ + 3) % 4];
            data_consumed_ += 4;
            //debugstd::cout << " to " << *it << *(it+1)<< *(it+2)<< *(it+3) << std::endl;
        }
    }
    //debugstd::cout << "here" << std::endl;
    //there can potentially be 4 bytes left in the data stream to be unmasked 1 byte at a time
    for(;it < end;++it) {
            //debugstd::cout << "consuming 1 byte " << *it;
            *it ^= mask[data_consumed_ % 4];
            ++data_consumed_;
            //debugstd::cout << " to " << *it <<  std::endl;
    }
    return data_consumed_;
}

template<class T>
void Frame<T>::flags2frame(uint16_t flags) {
    it_t frame = buffer_.begin() + frame_offset_;
    //frame_ is 8 bits
    frame[0]  = 0x00;
    frame[1] &= 0x7F;

    uint8_t flags_8bit = (flags & 0xff00) >> 8;

    frame[0] = 
        (flags_8bit & 0x80)
     |  (flags_8bit & 0x40)
     |  (flags_8bit & 0x20)
     |  (flags_8bit & 0x10)
     |  (flags_8bit & 0x00)
     |  (flags_8bit & 0x01)
     |  (flags_8bit & 0x02)
     |  (flags_8bit & 0x03)
     |  (flags_8bit & 0x09)
     |  (flags_8bit & 0x0A);
    
    flags_8bit = (flags & 0x00ff) >> 0;
    
    frame[1] = (frame[1] & 0x7F) | (flags_8bit & 0x80);

}

}

#endif
