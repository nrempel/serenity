/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
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

#include <LibGfx/Color.h>
#include <LibGfx/WindowTheme.h>

namespace Gfx {

class ClassicWindowTheme final : public WindowTheme {
public:
    ClassicWindowTheme();
    virtual ~ClassicWindowTheme() override;

    virtual void paint_normal_frame(Painter&, WindowState, const IntRect& window_rect, const StringView& title, const Bitmap& icon, const Palette&, const IntRect& leftmost_button_rect, int menu_row_count) const override;
    virtual void paint_tool_window_frame(Painter&, WindowState, const IntRect& window_rect, const StringView& title, const Palette&, const IntRect& leftmost_button_rect) const override;
    virtual void paint_notification_frame(Painter&, const IntRect& window_rect, const Palette&, const IntRect& close_button_rect) const override;

    virtual int titlebar_height(WindowType, const Palette&) const override;
    virtual IntRect titlebar_rect(WindowType, const IntRect& window_rect, const Palette&) const override;
    virtual IntRect titlebar_icon_rect(WindowType, const IntRect& window_rect, const Palette&) const override;
    virtual IntRect titlebar_text_rect(WindowType, const IntRect& window_rect, const Palette&) const override;

    virtual IntRect menubar_rect(WindowType, const IntRect& window_rect, const Palette&, int menu_row_count) const override;

    virtual IntRect frame_rect_for_window(WindowType, const IntRect& window_rect, const Palette&, int menu_row_count) const override;

    virtual Vector<IntRect> layout_buttons(WindowType, const IntRect& window_rect, const Palette&, size_t buttons) const override;
    virtual bool is_simple_rect_frame() const override { return true; }
    virtual bool frame_uses_alpha(WindowState state, const Palette& palette) const override
    {
        return compute_frame_colors(state, palette).uses_alpha();
    }
    virtual float frame_alpha_hit_threshold(WindowState) const override { return 1.0f; }

private:
    struct FrameColors {
        Color title_color;
        Color border_color;
        Color border_color2;
        Color title_stripes_color;
        Color title_shadow_color;

        bool uses_alpha() const
        {
            return title_color.alpha() != 0xff || border_color.alpha() != 0xff || border_color2.alpha() != 0xff || title_stripes_color.alpha() != 0xff || title_shadow_color.alpha() != 0xff;
        }
    };

    FrameColors compute_frame_colors(WindowState, const Palette&) const;
};

}
