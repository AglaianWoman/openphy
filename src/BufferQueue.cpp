/*
 * Interthread Buffer Queue
 *
 * Copyright (C) 2016 Ettus Research LLC
 * Author Tom Tsou <tom.tsou@ettus.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "BufferQueue.h"

size_t BufferQueue::size()
{
    std::lock_guard<std::mutex> guard(mutex);
    return q.size();
};

std::shared_ptr<LteBuffer> BufferQueue::read()
{
    std::lock_guard<std::mutex> guard(mutex);
    if (q.empty())
        return NULL;
    auto buf = q.front();
    q.pop();

    return buf;
}

bool BufferQueue::write(std::shared_ptr<LteBuffer> buf)
{
    std::lock_guard<std::mutex> guard(mutex);
    q.push(buf);
    return true;
}

BufferQueue::BufferQueue(const BufferQueue &q)
  : BufferQueue()
{
}

BufferQueue &BufferQueue::operator=(const BufferQueue &q)
{
}
