/*
 * See the LICENSE file at the root directory for license information
 */

/*
 * File:   oscsender.h
 * Author: frankiezafe
 *
 * Created on December 10, 2018, 1:21 PM
 */

#ifndef OSCSENDER_H
#define OSCSENDER_H

#include <scene/main/node.h>

#include "oscmsg.h"

namespace osc {

    class oscsender : public GodotScript<Node> {
        GODOT_CLASS(oscsender)

    public:

        static void _register_methods();

        oscsender();

        virtual ~oscsender();

        bool setup(String ip, unsigned int port);

        bool start();

        void stop();

        void msg( String address );

        void add( Variant var );

        void send();

    protected:

        std::string _ip;
        int _port;
        std::size_t _buffer_size;
        String _endpoint;
        bool _ready;
        bool _running;

        UdpTransmitSocket* _socket;

        char* _buffer;
        osc::OutboundPacketStream* _packet;
        bool _packet_closed;

        void reset_message();

    };
};

#endif /* OSCSENDER_H */
