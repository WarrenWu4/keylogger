// custom ui library for keylogger
#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include "font_manager.h"

namespace UI {
    enum FLEXDIRECTION {
        HORIZONTAL,
        VERTICAL
    };

    using POSITION = POINT;
    using PADDING = POINT;
    using GAP = POINT;

    struct SIZE {
        int width = 0;
        int height = 0;
    };
}

class Element {
private:
    Gdiplus::Rect rect = Gdiplus::Rect(0, 0, 0, 0);
    std::vector<std::shared_ptr<Element>> children;
    UI::FLEXDIRECTION direction = UI::VERTICAL;
    UI::PADDING padding = {0, 0};
    size_t gap = 0;

protected:
    void drawChildren(HDC hdc) {
        for (size_t i = 0; i < children.size(); i++) {
            children.at(i)->draw(hdc);
        }
    }

    void handleChildren(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        for (size_t i = 0; i < children.size(); i++) {
            children.at(i)->handler(hwnd, msg, wParam, lParam);
        }
    }

public:
    virtual void draw(HDC hdc) { drawChildren(hdc); }
    virtual void handler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) { handleChildren(hwnd, msg, wParam, lParam); }

    Gdiplus::Rect getRect() { return rect; }
    RECT getConvertedRect() { return RECT{rect.X, rect.Y, rect.GetRight(), rect.GetBottom()}; }
    UI::SIZE getSize() { return UI::SIZE{rect.Width, rect.Height}; }
    UI::POSITION getPosition() { return UI::POSITION{rect.X, rect.Y}; }
    int getWidth() { return rect.Width - padding.x * 3; }
    int getHeight() { return rect.Height - padding.y * 3; }
    Element& setRect(Gdiplus::Rect newRect) { rect = newRect; return *this; }
    Element& setPosition(UI::POSITION newPosition) { rect.X = newPosition.x; rect.Y = newPosition.y; return *this; }
    Element& setSize(UI::SIZE newSize) { rect.Width = newSize.width; rect.Height = newSize.height; return *this; }
    Element& setDirection(UI::FLEXDIRECTION newDirection) { direction = newDirection; return *this; }
    Element& setPadding(UI::PADDING newPadding) { padding = newPadding; return *this; }
    Element& setGap(size_t newGap) { gap = newGap; return *this; }

    std::vector<std::shared_ptr<Element>>& getChildren() { return children; }
    Element& addChild(const std::shared_ptr<Element>& newChild) { 
        UI::POSITION childPosition = newChild->getPosition();
        if (children.empty()) {
            newChild->setPosition({
                childPosition.x + rect.X + padding.x, 
                childPosition.y + rect.Y + padding.y
            });
        } else {
            Gdiplus::Rect lastChildRect = children.back()->getRect();
            if (direction == UI::VERTICAL) {
                newChild->setPosition({
                    childPosition.x + rect.X + padding.x,
                    static_cast<long>(lastChildRect.Y + lastChildRect.Height + gap)
                });
            } else {
                newChild->setPosition({
                    static_cast<long>(lastChildRect.X + lastChildRect.Width + gap),
                    childPosition.y + rect.Y + padding.y
                });
            }
        }
        children.push_back(newChild); 
        return *this; 
    }
    Element& removeChild(size_t childIndex) { 
        if (childIndex >= 0 && childIndex < children.size()) {
            children.erase(children.begin() + childIndex);
        }
        return *this; 
    }

    Element& centerVerticalFromElement(std::shared_ptr<Element> parent) {
        rect.Y = parent->getRect().Y + (parent->getRect().Height - rect.Height) / 2;
        return *this;
    }
    Element& centerHorizontalFromElement(std::shared_ptr<Element> parent) {
        rect.X = parent->getRect().X + (parent->getRect().Width - rect.Width) / 2;
        return *this;
    }
    Element& centerFromElement(std::shared_ptr<Element> parent) {
        centerVerticalFromElement(parent);
        centerHorizontalFromElement(parent);
        return *this;
    }
    Element& fitToChildren() {
        if (children.empty()) { return *this; } 
        std::shared_ptr<Element> lastChild = children.back();
        rect.Width = lastChild->getRect().GetRight() + padding.x;
        rect.Height = lastChild->getRect().GetBottom() + padding.y;
        return *this;
    }
};

class Box : public Element {
private:
    Gdiplus::Color backgroundColor = Gdiplus::Color(0, 0, 0, 0);
    Gdiplus::Color borderColor = Gdiplus::Color(0, 0, 0, 0);
    int borderRadius = 0;
    int borderWidth = 0;
public:
    Gdiplus::Color getBackgroundColor() { return backgroundColor; }
    Gdiplus::Color getBorderColor() { return borderColor; }
    int getBorderRadius() { return borderRadius; }
    int getBorderWidth() { return borderWidth; }
    Box& setBackgroundColor(Gdiplus::Color newBackgroundColor) { backgroundColor = newBackgroundColor; return *this; }
    Box& setBorderColor(Gdiplus::Color newBorderColor) { borderColor = newBorderColor; return *this; }
    Box& setBorderRadius(int newBorderRadius) { borderRadius = newBorderRadius; return *this; }
    Box& setBorderWidth(int newBorderWidth) { borderWidth = newBorderWidth; return *this; }
    void draw(HDC hdc) override {
        Gdiplus::Graphics graphics(hdc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

        Gdiplus::Rect rect = this->getRect();
        Gdiplus::SolidBrush backgroundBrush(backgroundColor);
        Gdiplus::Pen borderPen(borderColor, borderWidth);

        if (borderRadius > 0) {
            Gdiplus::GraphicsPath path;
            path.AddArc(rect.X, rect.Y, borderRadius * 2, borderRadius * 2, 180, 90);
            path.AddArc(rect.X + rect.Width - borderRadius * 2, rect.Y, borderRadius * 2, borderRadius * 2, 270, 90);
            path.AddArc(rect.X + rect.Width - borderRadius * 2, rect.Y + rect.Height - borderRadius * 2, borderRadius * 2, borderRadius * 2, 0, 90);
            path.AddArc(rect.X, rect.Y + rect.Height - borderRadius * 2, borderRadius * 2, borderRadius * 2, 90, 90);
            path.CloseFigure();
            graphics.FillPath(&backgroundBrush, &path);
            if (borderWidth > 0) {
                graphics.DrawPath(&borderPen, &path);
            }
        } else {
            graphics.FillRectangle(&backgroundBrush, rect);
            if (borderWidth > 0) {
                graphics.DrawRectangle(&borderPen, rect);
            }
        }

        drawChildren(hdc);
    }
};

class Text : public Element {
private:
    std::wstring text = L"";
    FontProperties fontProperties;
    Gdiplus::Color textColor = Gdiplus::Color(0, 0, 0, 255);
    std::shared_ptr<Gdiplus::StringFormat> alignment = std::make_shared<Gdiplus::StringFormat>(
        Gdiplus::StringFormat::GenericDefault()
    );

    std::unique_ptr<FontManager> fontManager = std::make_unique<FontManager>();

    void updateBoundingRect() {
        HDC hdc = GetDC(NULL);
        Gdiplus::Graphics graphics(hdc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        Gdiplus::RectF measurementFloat(0.0f, 0.0f, 9999.0f, 9999.0f);
        Gdiplus::RectF boundingRect;
        graphics.MeasureString(text.c_str(), -1, fontManager->getFont(fontProperties).get(), measurementFloat, alignment.get(), &boundingRect);
        setSize(UI::SIZE{(int)std::ceil(boundingRect.Width), (int)std::ceil(boundingRect.Height)});
    }

public:
    std::wstring getText() { return text; }
    Text& setText(std::wstring newText) { text = newText; updateBoundingRect(); return *this; }

    FontProperties getFontProperties() { return fontProperties; }
    Text& setFontProperties(FontProperties newFontProperties) { fontProperties = newFontProperties; updateBoundingRect(); return *this; }

    Gdiplus::Color getTextColor() { return textColor; }
    Text& setTextColor(Gdiplus::Color newTextColor) { textColor = newTextColor; return *this; }

    std::shared_ptr<Gdiplus::StringFormat> getTextAlignment() { return alignment; }
    Text& setTextAlignment(std::shared_ptr<Gdiplus::StringFormat> newAlignment) { alignment = newAlignment; return *this; }

    void draw(HDC hdc) override {
        Gdiplus::Graphics graphics(hdc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

        Gdiplus::SolidBrush textBrush(textColor);
        Gdiplus::RectF textRect = Gdiplus::RectF(
            (float)this->getRect().X,
            (float)this->getRect().Y,
            (float)this->getRect().Width,
            (float)this->getRect().Height
        );

        graphics.DrawString(text.c_str(), -1, fontManager->getFont(fontProperties).get(), textRect, alignment.get(), &textBrush);

        // for debugging purposes
        // Gdiplus::Pen debugPen(Gdiplus::Color(255, 0, 0, 255), 1);
        // graphics.DrawRectangle(&debugPen, getRect());
        // std::string message = "Bounding Rect X: " + std::to_string(boundingRect.X) + "\nBounding Rect Width: " + std::to_string(boundingRect.Width) + "\nElement X: " + std::to_string(getRect().X) + "\nElement Width: " + std::to_string(getRect().Width) + "\nCalcualted Width: " + std::to_string(getSize().width);
        // MessageBoxA(NULL, message.c_str(), "debug", MB_OK);

        drawChildren(hdc);
    }
};

class Circle : public Element {
private:
    Gdiplus::Color backgroundColor = Gdiplus::Color(0, 0, 0, 0);
    Gdiplus::Color borderColor = Gdiplus::Color(0, 0, 0, 0);
    int borderWidth = 0;
public:
    Gdiplus::Color getBackgroundColor() { return backgroundColor; }
    Gdiplus::Color getBorderColor() { return borderColor; }
    int getBorderWidth() { return borderWidth; }
    Circle& setBackgroundColor(Gdiplus::Color newBackgroundColor) { backgroundColor = newBackgroundColor; return *this; }
    Circle& setBorderColor(Gdiplus::Color newBorderColor) { borderColor = newBorderColor; return *this; }
    Circle& setBorderWidth(int newBorderWidth) { borderWidth = newBorderWidth; return *this; }
    void draw(HDC hdc) override {
        Gdiplus::Graphics graphics(hdc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

        Gdiplus::SolidBrush backgroundBrush(backgroundColor);
        Gdiplus::Pen borderPen(borderColor, borderWidth);

        graphics.FillEllipse(&backgroundBrush, this->getRect());
        if (borderWidth > 0) { graphics.DrawEllipse(&borderPen, this->getRect()); }

        drawChildren(hdc);
    }
};

// class Slider : public Element {
// private:
//     int value = 0;
//     std::shared_ptr<Text> label = std::make_shared<Text>();
//     std::shared_ptr<Box> track = std::make_shared<Box>();
//     std::shared_ptr<Circle> thumb = std::make_shared<Circle>();
//     bool dragging = false;

//     void syncThumbPosition() {
//         // ! syncs the thumb position to the value
//         POINT thumbRange = {
//             track->getRect().left + thumb->getSize().width / 2,
//             track->getRect().right - thumb->getSize().width / 2
//         };
//         int thumbOffset = thumbRange.x + (thumbRange.y - thumbRange.x) * value / 100.0f;
//         thumb->setPosition({
//             thumbOffset - thumb->getSize().width / 2,
//             thumb->getPosition().y
//         });
//     }

//     int getRelativeSliderValue(POINT cursorPos) {
//         RECT trackRect = track->getRect();
//         int relativeValue = (cursorPos.x - trackRect.left) * 100 / (trackRect.right - trackRect.left);
//         return std::clamp(relativeValue, 0, 100);
//     }
// public:
//     int getValue() { return value; }
//     Slider& setValue(int newValue) {
//         value = std::clamp(newValue, 0, 100);
//         label->setText(std::to_wstring(value) + L"%");
//         return *this;
//     }
//     Text& getLabel() { return *label; }
//     Box& getTrack() { return *track; }
//     Circle& getThumb() { return *thumb; }
//     void draw(HDC hdc) override {
//         syncThumbPosition();
//         label->draw(hdc);
//         track->draw(hdc);
//         thumb->draw(hdc);
//     }
//     void handler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override {
//         switch (msg) {
//             case WM_LBUTTONDOWN: {
//                 POINT cursorPos;
//                 GetCursorPos(&cursorPos);
//                 ScreenToClient(hwnd, &cursorPos);
//                 RECT thumbRect = thumb->getRect();
//                 if (PtInRect(&thumbRect, cursorPos)) {
//                     dragging = true;
//                     SetCapture(hwnd);
//                 }
//                 break;
//             }
//             case WM_MOUSEMOVE: {
//                 if (dragging) {
//                     POINT cursorPos;
//                     GetCursorPos(&cursorPos);
//                     ScreenToClient(hwnd, &cursorPos);
//                     setValue(getRelativeSliderValue(cursorPos));
//                     RECT thumbRect = thumb->getRect();
//                     InvalidateRect(hwnd, &thumbRect, TRUE);
//                     RECT textRect = label->getRect();
//                     // ! add a little extra padding to avoid cut off
//                     textRect.right += 20;
//                     InvalidateRect(hwnd, &textRect, TRUE);
//                 }
//                 break;
//             }
//             case WM_LBUTTONUP: {
//                 if (dragging) {
//                     dragging = false;
//                     ReleaseCapture();
//                 }
//                 break;
//             }
//         }
//     }
// };

// class Dropdown : public Element {
// private:
//     std::shared_ptr<Box> selectedContainer = std::make_shared<Box>();
//     std::shared_ptr<Text> selectedText = std::make_shared<Text>();

//     std::shared_ptr<Box> optionsContainer = std::make_shared<Box>();
//     std::vector<std::shared_ptr<Text>> optionElements;
//     std::vector<std::wstring> options;
//     bool showOptions = false;
//     int selectedIndex = 0;
//     HFONT font = nullptr;

// public:
//     Dropdown& setFont(HFONT newFont) {
//         font = newFont;
//         return *this;
//     }
//     std::shared_ptr<Box> getSelectedContainer() { return selectedContainer; }
//     std::shared_ptr<Text> getSelectedText() { return selectedText; }
//     std::shared_ptr<Box> getOptionsContainer() { return optionsContainer; }
//     std::vector<std::wstring> getOptions() { return options; }
//     Dropdown& setOptions(const std::vector<std::wstring>& newOptions) { 
//         options = newOptions;
//         optionElements.clear();
//         for (size_t i = 0; i < options.size(); i++) {
//             std::shared_ptr<Text> optionText = std::make_shared<Text>();
//             optionText->setText(options[i]).setFontSize(12).setTextColor(RGB(0, 0, 0));
//             optionElements.push_back(optionText);
//         }
//         return *this;
//     }
//     int getSelectedIndex() { return selectedIndex; }
//     Dropdown& setSelectedIndex(int newIndex) {
//         if (newIndex >= 0 && newIndex < static_cast<int>(options.size())) {
//             selectedIndex = newIndex;
//             selectedText->setText(options[selectedIndex]).centerFromElement(selectedContainer);
//         }
//         return *this;
//     }
//     void draw(HDC hdc) override {  
//         selectedContainer->draw(hdc);
//         selectedText->draw(hdc);
//         if (showOptions) {
//             optionsContainer->draw(hdc);
//             for (size_t i = 0; i < optionElements.size(); i++) {
//                 optionElements[i]->setFont(font).setPosition({
//                     0,
//                     optionsContainer->getPosition().y + 8 + static_cast<int>(i) * 36 
//                 }).horizontalCenterFromElement(optionsContainer);
//                 optionElements[i]->draw(hdc);
//             }
//         }
//     }
//     void handler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override {
//         if (msg == WM_LBUTTONDOWN) {
//             POINT cursorPos;
//             GetCursorPos(&cursorPos);
//             ScreenToClient(hwnd, &cursorPos);
//             RECT selectedRect = selectedContainer->getRect();
//             RECT optionsRect = optionsContainer->getRect();
//             if (PtInRect(&selectedRect, cursorPos)) {
//                 showOptions = !showOptions;
//                 InvalidateRect(hwnd, &optionsRect, TRUE);
//             }
//             if (PtInRect(&optionsRect, cursorPos) && showOptions) {
//                 int optionHeight = 36;
//                 int relativeY = cursorPos.y - optionsRect.top;
//                 int clickedIndex = relativeY / optionHeight;
//                 if (clickedIndex >= 0 && clickedIndex < static_cast<int>(options.size())) {
//                     setSelectedIndex(clickedIndex);
//                     showOptions = false;
//                     InvalidateRect(hwnd, &selectedRect, TRUE);
//                     InvalidateRect(hwnd, &optionsRect, TRUE);
//                 }
//             }
//         }
//     }
// };

class TextInput : public Element {
private:
    bool focus = false;
    std::wstring validCharacters = L"";
    std::wstring placeholder = L"";
    int minLength = 0;
    int maxLength = 50;

    std::shared_ptr<Box> boxEl = std::make_shared<Box>();
    std::shared_ptr<Text> textEl = std::make_shared<Text>();

    std::function<void()> onSubmit = nullptr;
    std::function<void()> onFocus = nullptr;
    std::function<void()> onBlur = nullptr;
    std::function<void()> onChange = nullptr;

    bool isValidChar(wchar_t inputChar) {
        return (validCharacters.find(inputChar) != std::wstring::npos);
    }

public:
    void init() {
        placeholder = L"16"; // ! temp
        boxEl->setBackgroundColor(Gdiplus::Color(255, 255, 255))
            .setBorderColor(Gdiplus::Color(0, 0, 0))
            .setBorderWidth(2)
            .setPadding({8, 8})
            .setSize({80, 40});
        textEl->setText(this->placeholder)
            .setFontProperties(FontProperties())
            .setTextColor(Gdiplus::Color(0, 0, 0));
        textEl->centerFromElement(boxEl);
        this->addChild(boxEl);
        boxEl->addChild(textEl);
    }

    TextInput& setText(std::wstring newText) { 
        textEl->setText(newText); 
        boxEl->fitToChildren();
        return *this; 
    }

    TextInput& setFocus(bool newFocus) { focus = newFocus; return *this; }
    bool isFocused() { return focus; }
    TextInput& setValidCharacters(std::wstring newValidCharacters) { validCharacters = newValidCharacters; return *this; }
    TextInput& setMaxLength(int newMaxLength) { maxLength = newMaxLength; return *this; }
    TextInput& setMinLength(int newMinLength) { minLength = newMinLength; return *this; }
    TextInput& setOnSubmit(std::function<void()> newOnSubmit) { onSubmit = newOnSubmit; return *this; }
    TextInput& setOnFocus(std::function<void()> newOnFocus) { onFocus = newOnFocus; return *this; }
    TextInput& setOnBlur(std::function<void()> newOnBlur) { onBlur = newOnBlur; return *this; }

    void handler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override {
        switch (msg) {
            case WM_LBUTTONDOWN: {
                POINT cursorPos;
                GetCursorPos(&cursorPos);
                ScreenToClient(hwnd, &cursorPos);
                RECT containerRect = this->getConvertedRect();
                // std::string msg = "Cursor X: " + std::to_string(cursorPos.x) + "\nCursor Y: " + std::to_string(cursorPos.y) + "\nContainer Left: " + std::to_string(containerRect.left) + "\nContainer Right: " + std::to_string(containerRect.right) + "\nContainer Top: " + std::to_string(containerRect.top) + "\nContainer Bottom: " + std::to_string(containerRect.bottom);
                // MessageBoxA(NULL, msg.c_str(), "debug", MB_OK);
                if (PtInRect(&containerRect, cursorPos)) {
                    assert(onFocus && onBlur);
                    if (focus) {
                        onBlur();
                    } else {
                        onFocus();
                    }
                    focus = !focus;
                    InvalidateRect(hwnd, &containerRect, TRUE);
                }
                break;
            }
            case WM_CHAR: {
                if (focus) {
                    switch (wParam) {
                        case VK_RETURN:
                        case VK_ESCAPE: {
                            focus = false;
                            if (onBlur) { onBlur(); }
                            if (onSubmit) { onSubmit(); }
                            RECT tempRect = this->getConvertedRect();
                            InvalidateRect(hwnd, &tempRect, TRUE);
                            break;
                        }
                        case VK_BACK: {
                            // std::wstring currentText = this->getText();
                            // if (currentText.length() > static_cast<size_t>(minLength)) {
                            //     currentText.pop_back();
                            //     this->setText(currentText);
                            //     RECT tempRect = this->getConvertedRect();
                            //     InvalidateRect(hwnd, &tempRect, TRUE);
                            // }
                            // break;
                        }
                        default: {
                            assert(onChange);
                            onChange();
                            // std::wstring currentText = this->getText();
                            // if (isValidChar(static_cast<wchar_t>(wParam)) && currentText.length() < static_cast<size_t>(maxLength)) {
                            //     this->setText(currentText + static_cast<wchar_t>(wParam));
                            //     RECT tempRect = this->getConvertedRect();
                            //     InvalidateRect(hwnd, &tempRect, TRUE);
                            // }
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
};