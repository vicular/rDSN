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
 *     What is this file about?
 *
 * Revision history:
 *     xxxx-xx-xx, author, first version
 *     xxxx-xx-xx, author, fix bug about xxx
 */

# include <dsn/internal/message_parser.h>
# include <dsn/service_api_c.h>

# ifdef __TITLE__
# undef __TITLE__
# endif
# define __TITLE__ "message.parser"

namespace dsn {

    message_parser::message_parser(int buffer_block_size, bool is_write_only)
        : _buffer_block_size(buffer_block_size)
    {
        if (!is_write_only)
        {
            create_new_buffer(buffer_block_size);
        }
    }

    void message_parser::create_new_buffer(unsigned int sz)
    {
        _read_buffer.assign(std::shared_ptr<char>(new char[sz], std::default_delete<char[]>{}), 0, sz);
        _read_buffer_occupied = 0;
    }

    void message_parser::mark_read(unsigned int read_length)
    {
        dassert(read_length + _read_buffer_occupied <= _read_buffer.length(), "");
        _read_buffer_occupied += read_length;
    }

    // before read
    void* message_parser::read_buffer_ptr(int read_next)
    {
        if (read_next + _read_buffer_occupied > _read_buffer.length())
        {
            // remember currently read content
            auto rb = _read_buffer.range(0, _read_buffer_occupied);
            
            // switch to next
            if (read_next + _read_buffer_occupied > _buffer_block_size)
                create_new_buffer(read_next + _read_buffer_occupied);
            else
                create_new_buffer(_buffer_block_size);

            // copy
            if (rb.length() > 0)
            {
                memcpy((void*)_read_buffer.data(), (const void*)rb.data(), rb.length());
                _read_buffer_occupied = rb.length();
            }            
            
            dassert (read_next + _read_buffer_occupied <= _read_buffer.length(), "");
        }

        return (void*)(_read_buffer.data() + _read_buffer_occupied);
    }

    unsigned int message_parser::read_buffer_capacity() const
    {
        return _read_buffer.length() - _read_buffer_occupied;
    }

    //-------------------- msg parser manager --------------------
    message_parser_manager::message_parser_manager()
    {
    }

    void message_parser_manager::register_factory(network_header_format fmt, message_parser::factory f, message_parser::factory2 f2, size_t sz)
    {
        if (static_cast<unsigned int>(fmt) >= _factory_vec.size())
        {
            _factory_vec.resize(fmt + 1);
        }

        parser_factory_info& info = _factory_vec[fmt];
        info.fmt = fmt;
        info.factory = f;
        info.factory2 = f2;
        info.parser_size = sz;
    }

    message_parser* message_parser_manager::create_parser(network_header_format fmt, int buffer_blk_size, bool is_write_only)
    {
        parser_factory_info& info = _factory_vec[fmt];
        return info.factory(buffer_blk_size, is_write_only);
    }

}
