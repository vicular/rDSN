/*
* The MIT License (MIT)
*
* Copyright (c) 2015 Microsoft Corporation
*
* -=- Robust Distributed System Nucleus (rDSN) -=-
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

/*
* Description:
*     message parser for browser-generated http request
*
* Revision history:
*     Jun. 2016, Zuoyan Qin, first version
*     xxxx-xx-xx, author, fix bug about xxx
*/

#pragma once

# include <dsn/internal/message_parser.h>
# include <dsn/internal/rpc_message.h>
# include <dsn/internal/ports.h>

# include "thrift_message_parser.h"
# include "dsn_message_parser.h"

namespace dsn
{
    class general_message_parser : public message_parser
    {
    public:
        general_message_parser(int buffer_block_size, bool is_write_only);

        virtual message_ex* get_message_on_receive(unsigned int read_length, /*out*/int& read_next) override;

        virtual void truncate_read() override;

        virtual void on_create_response(message_ex* request_msg, message_ex* response_msg) override;

        virtual int prepare_on_send(message_ex* msg) override;

        virtual int get_buffers_on_send(message_ex* msg, /*out*/ send_buf* buffers) override;

    private:
        // for thrift
        thrift_message_header _thrift_header;
        bool _header_parsed;

        // for dsn
        bool _header_checked;
    };
}
