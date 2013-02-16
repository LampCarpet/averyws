#ifndef WS_SESSION_MANAGER
#define WS_SESSION_MANAGER

#include <ws_session.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Websocket {

class SessionManager {
  public:
      void add_unauthed(const std::shared_ptr<Session> &session);
      void add_authed(const std::shared_ptr<Session> &session,const std::string &sid);
      void move_to_authed(const std::shared_ptr<Session> &session,const std::string &sid);

      void remove_authed(const std::string &sid);
      void remove_unauthed(const std::shared_ptr<Session> &session);

      void remove(const std::shared_ptr<Session> &session);

      const std::set<std::shared_ptr<Session> >& unauthed();
      const std::map<std::string, std::shared_ptr<Session> >& authed();
      const std::map<std::string, std::vector<std::string> >& userFeeds();
      const std::map<std::string, std::vector<std::weak_ptr<Session> > >& feedUsers();

  private:
    //unauthed + authed = everyone connected
    std::set<std::shared_ptr<Session> > unauthed_;
    std::map<std::string, std::shared_ptr<Session> > authed_;
    
    std::map<std::string, std::vector<std::string> > user_feeds_;

    //the session knows it's ID so it can be derived
    std::map<std::string, std::vector<std::weak_ptr<Session> > > feed_users_;

};

}
#endif
