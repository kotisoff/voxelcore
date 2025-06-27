#pragma once

#include "Button.hpp"

namespace gui {
    class Label;

    class SelectBox : public Button {
    public:
        struct Option {
            std::string value;
            std::wstring text;
        };
    private:
        std::vector<Option> options;
        Option selected {};
    public:
        SelectBox(
            GUI& gui,
            std::vector<Option>&& elements,
            Option selected,
            int contentWidth,
            const glm::vec4& padding
        );

        void setSelected(const Option& selected);

        const Option& getSelected() const;

        const std::vector<Option>& getOptions() const;

        void drawBackground(const DrawContext& pctx, const Assets&) override;
    };
}
