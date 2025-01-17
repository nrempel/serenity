/*
 * Copyright (c) 2021, The SerenityOS developers.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <AK/Span.h>
#include <AK/String.h>
#include <LibCore/Object.h>
#include <LibWebSocket/ConnectionInfo.h>

namespace WebSocket {

class AbstractWebSocketImpl : public Core::Object {
    C_OBJECT_ABSTRACT(AbstractWebSocketImpl);

public:
    virtual ~AbstractWebSocketImpl() override;
    explicit AbstractWebSocketImpl(Core::Object* parent = nullptr);

    virtual void connect(ConnectionInfo const&) = 0;

    virtual bool can_read_line() = 0;
    virtual String read_line(size_t size) = 0;

    virtual bool can_read() = 0;
    virtual ByteBuffer read(int max_size) = 0;

    virtual bool send(ReadonlyBytes) = 0;

    virtual bool eof() = 0;

    virtual void discard_connection() = 0;

    Function<void()> on_connected;
    Function<void()> on_connection_error;
    Function<void()> on_ready_to_read;
};

}
