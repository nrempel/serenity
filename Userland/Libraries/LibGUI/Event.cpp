/*
 * Copyright (c) 2020-2021, Andreas Kling <kling@serenityos.org>
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

#include <AK/StringBuilder.h>
#include <LibCore/MimeData.h>
#include <LibGUI/Action.h>
#include <LibGUI/Event.h>

namespace GUI {

DropEvent::DropEvent(const Gfx::IntPoint& position, const String& text, NonnullRefPtr<Core::MimeData> mime_data)
    : Event(Event::Drop)
    , m_position(position)
    , m_text(text)
    , m_mime_data(move(mime_data))
{
}

DropEvent::~DropEvent()
{
}

String KeyEvent::to_string() const
{
    Vector<String, 8> parts;

    if (m_modifiers & Mod_Ctrl)
        parts.append("Ctrl");
    if (m_modifiers & Mod_Shift)
        parts.append("Shift");
    if (m_modifiers & Mod_Alt)
        parts.append("Alt");
    if (m_modifiers & Mod_Super)
        parts.append("Super");

    if (auto* key_name = key_code_to_string(static_cast<KeyCode>(m_key)))
        parts.append(key_name);
    else
        parts.append("(Invalid)");

    StringBuilder builder;
    for (size_t i = 0; i < parts.size(); ++i) {
        builder.append(parts[i]);
        if (i != parts.size() - 1)
            builder.append('+');
    }
    return builder.to_string();
}

ActionEvent::ActionEvent(Type type, Action& action)
    : Event(type)
    , m_action(action)
{
}

ActionEvent::~ActionEvent()
{
}

}
