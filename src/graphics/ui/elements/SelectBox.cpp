#include "SelectBox.hpp"

#include "Label.hpp"

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
}
