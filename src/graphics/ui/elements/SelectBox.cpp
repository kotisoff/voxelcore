#include "SelectBox.hpp"

#include "Label.hpp"
#include "assets/Assets.hpp"
#include "graphics/ui/GUI.hpp"
#include "graphics/ui/elements/Panel.hpp"
#include "graphics/core/Batch2D.hpp"
#include "graphics/core/DrawContext.hpp"

using namespace gui;

SelectBox::SelectBox(
    GUI& gui,
    std::vector<Element>&& elements,
    Element selected,
    int contentWidth,
    const glm::vec4& padding
)
    : Button(gui, selected.text, padding, nullptr, glm::vec2(contentWidth, -1)),
      elements(std::move(elements)) {

    listenAction([this](GUI& gui) {
        auto panel = std::make_shared<Panel>(gui, getSize());
        panel->setPos(calcPos() + glm::vec2(0, size.y));
        for (const auto& option : this->elements) {
            auto button = std::make_shared<Button>(
                gui, option.text, glm::vec4(10.0f), nullptr, glm::vec2(-1.0f)
            );
            button->listenFocus([this, option](GUI&) {
                setSelected(option);
            });
            panel->add(button);
        }
        panel->setZIndex(999);
        gui.setFocus(panel);
        panel->listenDefocus([panel=panel.get()](GUI& gui) {
            gui.remove(panel);
        });
        gui.add(panel);
    });
}

void SelectBox::setSelected(const Element& selected) {
    this->selected = selected;
    this->label->setText(selected.text);
}

void SelectBox::drawBackground(const DrawContext& pctx, const Assets&) {
    glm::vec2 pos = calcPos();
    auto batch = pctx.getBatch2D();
    batch->texture(nullptr);
    batch->setColor(calcColor());
    batch->rect(pos.x, pos.y, size.x, size.y);
    batch->setColor({1.0f, 1.0f, 1.0f, 0.333f});
    batch->triangle(
        pos.x + size.x - 32, pos.y + size.y / 2.0f - 4,
        pos.x + size.x - 32 + 16, pos.y + size.y / 2.0f - 4,
        pos.x + size.x - 32 + 8, pos.y + size.y / 2.0f + 4
    );
}
