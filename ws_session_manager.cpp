#include <ws_session_manager.hpp>

#include <ws_session.hpp>
#include <memory>
#include <string>
#include <map>
#include <set>

namespace Websocket {

void SessionManager::add_unauthed(const std::shared_ptr<Session> &session) {
    unauthed_.insert(session);
}
void SessionManager::add_authed(const std::shared_ptr<Session> &session,const std::string &sid) {
    unauthed_.erase(session);
    if(authed_.find(sid) == authed_.end()) {
        authed_[sid] = session;
    } else {
        //todo
        std::cout << "add_unauthed failed, authed sid already exists" << std::endl;
    }
}

void SessionManager::move_to_authed(const std::shared_ptr<Session> &session,const std::string &sid) {
    unauthed_.erase(session);
    if(authed_.find(sid) == authed_.end()) {
        authed_[sid] = session;
    } else {
        //todo
        std::cout << "move_to_authed failed, authed sid already exists" << std::endl;
    }
}

void SessionManager::remove_authed(const std::string &sid) {
   authed_.erase(sid);
}

void SessionManager::remove_unauthed(const std::shared_ptr<Session> &session) {
   unauthed_.erase(session);
}
      
void SessionManager::remove(const std::shared_ptr<Session> &session) {
    std::cout << "removed session" << std::endl;
    if(session->authenticated()) {
        authed_.erase(session->sid());
    } else {
        unauthed_.erase(session);
    }
}

const std::set<std::shared_ptr<Session> >& SessionManager::unauthed() {
    return unauthed_;
}

const std::map<std::string, std::shared_ptr<Session> >& SessionManager::authed() {
    return authed_;
}

const std::map<std::string, std::vector<std::string> >& SessionManager::userFeeds() {
    return user_feeds_;
}

const std::map<std::string, std::vector<std::weak_ptr<Session> > >& SessionManager::feedUsers() { 
    return feed_users_;
}

}
