#ifndef THEME_HPP
#define THEME_HPP

#include <QPalette>

inline QPalette makeLightPalette() {
    QPalette p;
    p.setColor(QPalette::Window, QColor("#ffffff"));
    p.setColor(QPalette::WindowText, QColor("#202124"));
    p.setColor(QPalette::Base, QColor("#ffffff"));
    p.setColor(QPalette::AlternateBase, QColor("#f1f3f4"));
    p.setColor(QPalette::Text, QColor("#202124"));
    p.setColor(QPalette::Button, QColor("#f1f3f4"));
    p.setColor(QPalette::ButtonText, QColor("#202124"));
    p.setColor(QPalette::Highlight, QColor("#3b82f6"));
    p.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    return p;
}

inline QPalette makeDarkPalette() {
    QPalette p;
    p.setColor(QPalette::Window, QColor("#121212"));
    p.setColor(QPalette::WindowText, QColor("#eaeaea"));
    p.setColor(QPalette::Base, QColor("#1c1c1c"));
    p.setColor(QPalette::AlternateBase, QColor("#222222"));
    p.setColor(QPalette::Text, QColor("#eaeaea"));
    p.setColor(QPalette::Button, QColor("#222222"));
    p.setColor(QPalette::ButtonText, QColor("#eaeaea"));
    p.setColor(QPalette::Highlight, QColor("#3b82f6"));
    p.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    return p;
}

#endif // THEME_HPP