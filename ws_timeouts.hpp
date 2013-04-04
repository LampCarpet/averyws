#ifndef WS_SESSION
#define WS_SESSION

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace Websocket {

using namespace boost::asio;

class Timeouts {
    public:
      Timeouts(time_duraction
         time_duration connection
        ,time_duration transfer_baseline
        ,uint64_t min_transfer_rate_KBps
        ,time_duration global_maximum) :
             connection_(connection)
            ,transfer_baseline_(transfer_baseline)
            ,min_transfer_rate_KBps_(min_transfer_rate_KBps)
            ,global_maximum_(global_maximum) {}
      time_duration connection_timeout() const {
        return connection_; 
      }
      
      time_duration transfer_timeout(uint64_t bytes) const {
        return transfer_basline + microseconds(bytes * 1000000 / min_transfer_rate_KBps_ );
      }

      time_duration global_timeout() const {
          return global_maximum_;
      }

    private:
      time_duration connection_;
      time_duration transfer_baseline_;
      uint64_t min_transfer_rate_KBps_;
      time_duration global_maximum_;

};
}

#endif
