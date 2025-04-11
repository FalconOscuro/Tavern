#include "file/tpk_file.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include <boost/log/trivial.hpp>

namespace tavern::file {

tpk_file::tpk_file(const std::string_view tpk_path, const mount_path& file_path, const tpk::file_node* node, const size_t data_start):
    ifile(file_path), m_node(node), m_tpk_path(tpk_path), m_data_start(data_start)
{
    assert(node != nullptr);
}

tpk_file::~tpk_file() {
    close();
}

bool tpk_file::open()
{
    if (is_open())
        return true;

    m_file = fopen(m_tpk_path.c_str(), "rb");
    // disable buffering
    setbuf(m_file, nullptr);
    m_buffer = new char[get_buffer_size()]();
    m_file_pos = 0;
    
    if (is_open() && reset_buffer())
        return true;

    close();
    return false;
}

void tpk_file::close()
{
    if (!is_open())
        return;

    if (m_file != NULL) {
        fclose(m_file);
        m_file = NULL;
    }

    delete[] m_buffer;
    m_buffer = nullptr;

    m_file_pos = 0;
    m_buffer_pos = 0;
    m_buffer_head = 0;
    m_buffer_tail = 0;
}

bool tpk_file::eof() const {
    return pos() >= m_node->size;
}

bool tpk_file::is_open() const {
    // fully buffered files do not need to keep file open
    return m_file != NULL || fully_buffered();
}

char tpk_file::peek_char(const size_t offset) const
{
    if (!is_open() || eof())
        return EOF;

    // NOTE: Can trigger for end of fully buffered file if peeking from start
    // this is an edge case and should never occur if properly used
    else if (offset > get_buffer_size() - 1) {
        assert(false && "Improper useage of peek_char, cannot peek further than buffer size!");
        return EOF;
    }

    // don't need to populate buffer as this is garuanteed to be end of file
    else if (pos() + offset >= size())
        return EOF;

    else if (!populate_buffer(offset))
        return EOF;

    return m_buffer[m_buffer_pos];
}

char tpk_file::get_char()
{
    if (!is_open() || eof())
        return EOF;

    else if (!populate_buffer(1))
        return EOF;

    const char c = m_buffer[m_buffer_pos];
    ++m_file_pos;

    m_buffer_pos = (m_buffer_pos + 1) % get_buffer_size();

    return c;
}

size_t tpk_file::get_str(char* const s, size_t len)
{
    size_t len_requested = len;

    if (!is_open())
        return 0;

    // clamp to prevent read beyond file end
    len = std::min(len, size() - pos());

    const size_t buffer_size = get_buffer_size();

    if (fully_buffered())
    {
        memcpy(s, m_buffer + m_buffer_pos, len);
        m_buffer_pos += len;
        m_file_pos += len;
    }

    else if (len < (buffer_size - 1))
    {
        // read available data to prevent edge case where buffer head can be pushed beyond current buffer pos
        size_t bytes_read = std::min(get_buffer_used_size() - get_buffer_abs_pos(), len);
        
        size_t read_back, read_front;
        read_back  = std::min(bytes_read, buffer_size - m_buffer_pos);
        read_front = bytes_read - read_back;

        memcpy(s, m_buffer + m_buffer_pos, read_back);
        memcpy(s + read_back, m_buffer, read_front);

        m_buffer_pos = (m_buffer_pos + bytes_read) % buffer_size;
        m_file_pos += bytes_read;

        // data was fully available from already buffered, exit early
        if (bytes_read == len)
            return len;

        len -= bytes_read;
        if (!populate_buffer(len))
            len = get_buffer_used_size() - get_buffer_abs_pos();

        // bytes to read from end/back of circular buffer
        read_back  = std::min(len, buffer_size - m_buffer_pos);
        read_front = len - read_back;

        memcpy(s + bytes_read, m_buffer + m_buffer_pos, read_back);
        memcpy(s + bytes_read + read_back, m_buffer, read_front);

        m_file_pos += len;
        m_buffer_pos = (m_buffer_pos + len) % buffer_size;

        len += bytes_read;
    }

    // len longer than buffer size
    else {
        size_t bytes_read = 0;

        // read any available bytes from the buffer
        const size_t buffer_abs_pos = get_buffer_abs_pos();
        const size_t buffered_data = get_buffer_used_size() - buffer_abs_pos;

        size_t read_back = std::min(buffered_data, buffer_size - buffer_abs_pos);
        size_t read_front = buffered_data - read_back;

        memcpy(s, m_buffer + m_buffer_pos, read_back);
        memcpy(s + read_back, m_buffer, read_front);

        bytes_read += buffered_data;
        m_file_pos += bytes_read;
        len        -= bytes_read;

        const size_t new_pos = std::min(size() - (buffer_size - 1), m_file_pos + len - read_backward_bias());

        const size_t direct_read = new_pos - m_file_pos;
        const size_t bytes_read_direct = fread(s + bytes_read, sizeof(char), direct_read, m_file);

        bytes_read += bytes_read_direct;
        len        -= bytes_read_direct;
        m_file_pos = new_pos + len;

        // EOF?
        if (direct_read != bytes_read_direct)
            return bytes_read;

        reset_buffer();
        len = std::min(len, m_buffer_pos);

        memcpy(s + bytes_read, m_buffer, len);
        len += bytes_read;
    }

    assert(((size() - pos()) >= (get_buffer_used_size() - get_buffer_abs_pos()) || fully_buffered()) && "Buffer misalignment!");;

    if (len != len_requested)
        BOOST_LOG_TRIVIAL(warning) << "Failed to read all bytes from " << get_path() << ", expected '" << len_requested << "' but only found '" << len << '\'';
    return len;
}

// WARNING: Double check for safety
bool tpk_file::seek(const long offset, const origin mode)
{
    if (!is_open())
        return false;

    long seek_pos;
    switch (mode)
    {
    case START:
        seek_pos = 0;
        break;

    case END:
        seek_pos = size() - 1;
        break;

    default:
        seek_pos = m_file_pos;
        break;
    }

    const long offset_actual = std::clamp(offset, -(seek_pos), (long)size() - seek_pos);

    const size_t buffer_used = get_buffer_used_size();

    const size_t new_pos  = seek_pos + offset_actual;
    const long pos_offset = (long)new_pos - (long)m_file_pos;
    m_file_pos = new_pos;

    if ((size_t)std::abs(pos_offset) > buffer_used)
        return reset_buffer();

    else {
        const bool success = populate_buffer(pos_offset);
        m_buffer_pos = ((long)m_buffer_pos + pos_offset) % get_buffer_size();
        return success;
    }
}

size_t tpk_file::pos() const {
    return m_file_pos;
}

size_t tpk_file::size() const {
    return m_node->size;
}

size_t tpk_file::get_buffer_size() const {
    // Do not need to buffer more than file size
    return std::min<size_t>(size(), BUFSIZ);
}

size_t tpk_file::get_buffer_used_size() const {
    return ((long long)m_buffer_tail - (long long)m_buffer_head) % get_buffer_size();
}

size_t tpk_file::get_buffer_abs_pos() const {
    return ((long long)m_buffer_pos - (long long)m_buffer_head) % get_buffer_size();
}

bool tpk_file::populate_buffer(long offset) const
{
    // file fully populates buffer
    if (fully_buffered())
        return true;

    const size_t buffer_size = get_buffer_size();
    const size_t file_size   = size();

    const size_t buffer_used = get_buffer_used_size();
    const size_t buffer_abs_pos = get_buffer_abs_pos();

    const size_t file_pos = pos();
    const size_t file_end_dist = file_size - (file_pos + buffer_used - buffer_abs_pos);

    // clamp to not read beyond file end
    offset = std::clamp<long>(offset, -(long)file_pos, file_end_dist);

    assert(offset < (long)buffer_size && "Offset greater than buffer size!");
    size_t read;

    // seek forward
    if (offset > 0 && offset + buffer_abs_pos > buffer_used)
    {
        read = std::min(file_end_dist,     // do not read past file end
            std::max(
                read_forward_bias(),
                (offset + buffer_abs_pos) - buffer_used // ensure offset fully readable
        ));
        assert(read <= buffer_abs_pos && "read from offset would push head beyond current buffer pos");

        // bytes to append to tail, ensuring not to write beyond buffer end
        const size_t read_back = std::min(read, buffer_size - m_buffer_tail);

        size_t bytes_read = fread(m_buffer + m_buffer_tail, sizeof(char), read_back, m_file);
        // complete read by adding remaining bytes to start of buffer
        bytes_read += fread(m_buffer, sizeof(char), read - read_back, m_file);

        assert(bytes_read == read && "Hit unexpected file end!");
        if (bytes_read != read) {
            BOOST_LOG_TRIVIAL(error) << "Hit unexpected file end!";
            return false;
        }

        m_buffer_tail = (m_buffer_tail + read) % buffer_size;
        // only move head if necessary
        // is head needed?
        //if (read + buffer_abs_pos >= buffer_size)
        m_buffer_head = (m_buffer_head + read) % buffer_size;
    }

    // seek back
    else if (offset < 0 && (size_t)-offset > buffer_abs_pos)
    {
        read = std::min(file_pos,  // do not read before file start
            std::max<size_t>(
                read_backward_bias(),
                -offset                         // ensure offset fully readable
        ));
        assert(read <= buffer_used - buffer_abs_pos && "offset would push tail over current buffer pos");

        const size_t read_front = std::min(read, m_buffer_head);
        const size_t read_back  = read - read_front;
        fseek(m_file, -(long)(read + buffer_size), SEEK_CUR);

        size_t bytes_read = fread(m_buffer + buffer_size - read_back, sizeof(char), read_back, m_file);
        bytes_read += fread(m_buffer + m_buffer_head - read_front, sizeof(char), read_front, m_file);

        // seek to place actual file position according to buffer end
        fseek(m_file, std::max(0L, (long)buffer_size - (long)read), SEEK_CUR);

        assert(bytes_read == read && "Hit unexpected file start!");
        if (bytes_read != read)
        {

            BOOST_LOG_TRIVIAL(error) << "Hit unexpected file start!";
            return false;
        }

        m_buffer_head = ((long long)m_buffer_head - (long long)read) % buffer_size;
        // only move tail if necessary
        if (read + buffer_abs_pos >= buffer_size)
            m_buffer_tail = ((long long)m_buffer_tail - (long long)read) % buffer_size;
    }

    assert(((size() - pos()) >= (get_buffer_used_size() - get_buffer_abs_pos()) || fully_buffered()) && "Buffer misalignment!");;
    return true;
}

bool tpk_file::reset_buffer()
{
    const size_t file_size = size();

    // seek to file start
    if (fseek(m_file, file_start_pos(), SEEK_SET) != 0)
        return false;

    // fully buffered file
    if (fully_buffered())
    {
        const size_t bytes_read = fread(m_buffer, sizeof(char), file_size, m_file);
        // entirety of file is in the buffer, can be safely closed
        fclose(m_file);
        m_file = NULL;

        if (bytes_read != file_size)
        {
            BOOST_LOG_TRIVIAL(error) << "Hit unexpected file end in fully buffered file: " << get_path();
            return false;
        }

        m_buffer_head = 0;
        m_buffer_tail = file_size + 1;
        m_buffer_pos = 0;
    }

    else {
        const size_t file_pos = pos();

        // read as far forward and back as possible with bias
        const size_t read_forward_min  = std::min(read_forward_bias(), file_size - file_pos); 
        const size_t read_backward_min = std::min(read_backward_bias(), file_pos);

        size_t read_forward, read_backward;

        // Account for less than default backward bias to start of file
        if (read_backward_min == file_pos) {
            read_backward = read_backward_min;
            read_forward = (get_buffer_size() - 1) - read_backward_min;
        }

        else {
            read_forward = read_forward_min;
            read_backward = (get_buffer_size() - 1) - read_forward_min;
        }

        const size_t read = read_forward + read_backward;

        if (fseek(m_file, file_pos - read_backward, SEEK_CUR) != 0)
            return false;

        const size_t bytes_read = fread(m_buffer, sizeof(char), read, m_file);

        assert(bytes_read == read && "Hit unexpected file end");
        if (bytes_read != read) {
            BOOST_LOG_TRIVIAL(error) << "Hit unexpected file end in: " << get_path();
            return false;
        }

        m_buffer_head = 0;
        m_buffer_tail = read;
        m_buffer_pos = read_backward;
    }

    return true;
}

} /* namespace tavern::file */
