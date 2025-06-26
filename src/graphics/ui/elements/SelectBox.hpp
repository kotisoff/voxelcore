#pragma once

#include "Button.hpp"

namespace gui {
    class Label;

    class SelectBox : public Button {
    public:
        struct Element {
            std::string value;
            std::wstring text;
        };
    private:
        std::vector<Element> elements;
        Element selected {};
    public:
        SelectBox(
            GUI& gui,
            std::vector<Element>&& elements,
            Element selected,
            int contentWidth,
            const glm::vec4& padding
        );

        void setSelected(const Element& selected);

        void drawBackground(const DrawContext& pctx, const Assets&) override;
    };
}
