/*
 * See the LICENSE file at the root directory for license information
 */

/*
 * File:   oscreceiver.cpp
 * Author: frankiezafe
 *
 * Created on December 10, 2018, 5:41 PM
 */

#include <map>

#include "oscreceiver.h"

using namespace std;
using namespace osc;

void oscreceiver::_register_methods()
{

	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_queue"), "set_max_queue", "get_max_queue");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "avoid_duplicate"), "set_avoid_duplicate", "get_avoid_duplicate");
    //register_method("max_queue", &oscreceiver::max_queue);
    //register_method("avoid_duplicate", &oscreceiver::avoid_duplicate);

	ClassDB::bind_method(D_METHOD("setup"), &oscmsg::setup);
	ClassDB::bind_method(D_METHOD("start"), &oscmsg::start);
	ClassDB::bind_method(D_METHOD("stop"), &oscmsg::stop);
	ClassDB::bind_method(D_METHOD("has_message"), &oscmsg::has_message);
	ClassDB::bind_method(D_METHOD("get_next"), &oscmsg::get_next);
}

oscreceiver::oscreceiver() :
_port(0),
_ready(false), _running(false),
_swap_needed(true), _avoid_duplicate(false),
_lsocket(0),
_max_queue(100),
_queue_write(0),
_queue_read(0),
_qread_index(0), _qread_len(0) {
}

oscreceiver::~oscreceiver() {
    stop();
}

void oscreceiver::set_max_queue(int max_queue) {

    if (max_queue < 1) return;
    _lmutex.lock();
    _max_queue = (std::size_t)max_queue;
    _lmutex.unlock();
    check_queue();

}

void oscreceiver::set_avoid_duplicate(bool enabled) {

    _lmutex.lock();
    _avoid_duplicate = enabled;
    _lmutex.unlock();

}

bool oscreceiver::setup(unsigned int port) {

    if (port <= 0) {
        print("Invalid osc::oscreceiver setup: invalid port");
        return false;
    }

    bool autorestart = false;
    if (_running) {
        stop();
        autorestart = true;
    }

    _port = port;

    _ready = true;

    if (autorestart) {
        return start();
    }

    return true;

}

bool oscreceiver::start() {

    if (!_ready) {
        print("oscreceiver::start, receiver is not ready, "
                "call setup() first!");
        return false;
    }

    stop();

    try {
        IpEndpointName name(IpEndpointName::ANY_ADDRESS, _port);
        _lsocket = new UdpListeningReceiveSocket(name, this);
    } catch (const std::exception& e) {
        String s = "oscreceiver::start, failed to start on ";
        s += String::num(_port);
        s += "\n";
        s += e.what();
        print(s);
        return false;
    }

    create_buffers();

    _lthread = std::thread([this] {
        while (_lsocket) {
            try {
                _lsocket->Run();
                std::this_thread::yield();
            } catch (std::exception& e) {
                String s = "oscreceiver::_lthread, cannot listen ";
                s += e.what();
                print(s);
            }
        }
        String s = "oscreceiver::_lthread, thread is quitting on ";
        s += String::num(_port);
        print(s);
    });

    _lthread.detach();

    _running = true;
    String s = "oscreceiver::start, successfully started on ";
    s += String::num(_port);
    print(s);

    return true;

}

void oscreceiver::stop() {

    if (_lsocket) {
        _lsocket->Break();
        delete _lsocket;
        _lsocket = 0;
    }

    if (_running) {

        purge_buffers();

        _running = false;

        String s = "oscreceiver::stop, stopped on ";
        s += String::num(_port);
        print(s);

    }
}

void oscreceiver::create_buffers() {

    _lmutex.lock();
    if (!_queue_write) {
        _queue_write = new std::deque<oscmsg_data>();
        _queue_read = new std::deque<oscmsg_data>();
    }
    _lmutex.unlock();

}

void oscreceiver::purge_buffers() {

    if (_queue_write) {
        _queue_write->clear();
        _queue_read->clear();
        delete _queue_write;
        delete _queue_read;
        _queue_write = 0;
        _queue_read = 0;
    }

}

void oscreceiver::swap_buffers() {

    _queue_read->clear();

    _lmutex.lock();
    std::deque<oscmsg_data>* tmp = _queue_write;
    _queue_write = _queue_read;
    _queue_read = tmp;
    _lmutex.unlock();

    _qread_index = 0;
    _qread_len = _queue_read->size();

}

void oscreceiver::ProcessMessage(
        const osc::ReceivedMessage& m,
        const IpEndpointName& rep) {

    oscmsg_data msg(m, rep);
    if (msg.valid) {
        _lmutex.lock();
        _queue_write->push_back(msg);
        _lmutex.unlock();
        check_queue();
    }

}

void oscreceiver::check_queue() {

    _lmutex.lock();
    if (_queue_write && _queue_write->size() > _max_queue) {
        _queue_write->resize(_max_queue);
    }
    _lmutex.unlock();

}

void oscreceiver::purge_duplicates() {

    if ( _queue_read->size() < 2 ) {
        return;
    }

    std::map< String, oscmsg_data > map;

    std::deque<oscmsg_data> tmp;
    tmp.swap(*_queue_read);
    size_t tmp_size = tmp.size();

    std::deque<oscmsg_data>::iterator it = tmp.begin();
    std::deque<oscmsg_data>::iterator ite = tmp.end();
    for (; it != ite; ++it) {
        map[ (*it).data["address"] ] = (*it);
    }

    std::map< String, oscmsg_data >::iterator mi = map.begin();
    std::map< String, oscmsg_data >::iterator mie = map.end();
    for (; mi != mie; ++mi) {
        _queue_read->push_back(mi->second);
    }

    // updating the indices
    _qread_index = 0;
    _qread_len = _queue_read->size();

}

bool oscreceiver::has_message() {

    if (_swap_needed) {
        swap_buffers();
        if (_avoid_duplicate) {
            purge_duplicates();
        }
        _swap_needed = false;
    }

    if (_qread_index == _qread_len) {
        _swap_needed = true;
        return false;
    }

    return true;

}

Dictionary oscreceiver::get_next() {

    if (_qread_index == _qread_len) {
        return Dictionary();
    }
    return _queue_read->at(_qread_index++).data;

}
