// custom ui library for keylogger
#pragma once

#include <windows.h>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
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

// * might be able to reduce data storage
// * by only using RECT since other structs can be derived
// * but this is easier to understand
class Element {
private:
    RECT rect = {0, 0, 0, 0};
    std::vector<std::shared_ptr<Element>> children;
    UI::POSITION position = {0, 0};
    UI::SIZE size = {0, 0};
    UI::PADDING padding = {0, 0};

    void syncRect() {
        rect.left = position.x;
        rect.top = position.y;
        rect.right = position.x + size.width;
        rect.bottom = position.y + size.height;
    }
public:
    virtual void draw(HDC hdc) {};
    virtual void handler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {};
    virtual void drawStart(HDC hdc) {
        draw(hdc);
        for (size_t i = 0; i < children.size(); i++) {
            children[i]->drawStart(hdc);
        }
    }
    virtual void handlerStart(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        handler(hwnd, msg, wParam, lParam);
        for (size_t i = 0; i < children.size(); i++) {
            children[i]->handlerStart(hwnd, msg, wParam, lParam);
        }
    }
    RECT getRect() { return rect; }
    Element& setRect(RECT newRect) {
        rect = newRect; 
        position = {rect.left, rect.top};
        size = {rect.right - rect.left, rect.bottom - rect.top};
        return *this;
    }
    std::vector<std::shared_ptr<Element>>& getChildren() { return children; }
    int getLastChildBottom() {
        if (children.size() == 0) { return 0; }
        RECT lastChildRect = children.back()->getRect();
        return lastChildRect.bottom;
    }
    Element& addChild(const std::shared_ptr<Element>& newChild) { children.push_back(newChild); return *this; }
    Element& removeChild(size_t childIndex) { 
        if (childIndex >= 0 && childIndex < children.size()) {
            children.erase(children.begin() + childIndex);
        }
        return *this; 
    }
    UI::POSITION getPosition() { return position; }
    Element& setPosition(UI::POSITION newPosition) { 
        position = newPosition; 
        syncRect();
        return *this; 
    }
    UI::SIZE getSize() { return size; }
    Element& setSize(UI::SIZE newSize) { 
        size = newSize; 
        syncRect();
        return *this; 
    }
    UI::PADDING getPadding() { return padding; }
    Element& setPadding(UI::PADDING newPadding) { 
        padding = newPadding; 
        this->setPosition({
            position.x + padding.x,
            position.y + padding.y
        });
        this->setSize({
            // ! x3 to account for padding position change
            size.width - padding.x * 3,
            size.height - padding.y * 3
        });
        return *this; 
    }
    void horizontalCenterFromElement(std::shared_ptr<Element> element) {
        if (element == nullptr) { return; }
        UI::POSITION elPosition = element->getPosition();
        UI::SIZE elSize = element->getSize();
        this->setPosition({
            (elPosition.x + elSize.width/2) - (size.width) / 2,
            position.y
        });
    }
    void verticalCenterFromElement(std::shared_ptr<Element> element) {
        if (element == nullptr) { return; }
        UI::POSITION elPosition = element->getPosition();
        UI::SIZE elSize = element->getSize();
        this->setPosition({
            position.x,
            (elPosition.y + elSize.height/2) - (size.height) / 2
        });
    }
    void centerFromElement(std::shared_ptr<Element> element) {
        horizontalCenterFromElement(element);
        verticalCenterFromElement(element);
    }
};

class Box : public Element {
private:
    COLORREF backgroundColor = TRANSPARENT;
    COLORREF borderColor = TRANSPARENT;
    int borderRadius = 0;
    int borderWidth = 0;
public:
    COLORREF getBackgroundColor() { return backgroundColor; }
    COLORREF getBorderColor() { return borderColor; }
    int getBorderRadius() { return borderRadius; }
    int getBorderWidth() { return borderWidth; }
    Box& setBackgroundColor(COLORREF newBackgroundColor) { backgroundColor = newBackgroundColor; return *this; }
    Box& setBorderColor(COLORREF newBorderColor) { borderColor = newBorderColor; return *this; }
    Box& setBorderRadius(int newBorderRadius) { borderRadius = newBorderRadius; return *this; }
    Box& setBorderWidth(int newBorderWidth) { borderWidth = newBorderWidth; return *this; }
    void draw(HDC hdc) override {
        HBRUSH background = CreateSolidBrush(backgroundColor);
        HPEN border = CreatePen(PS_SOLID, borderWidth, borderColor);
        HGDIOBJ oldBackground = SelectObject(hdc, background);
        HGDIOBJ oldBorder = SelectObject(hdc, border);
        RECT rect = this->getRect();
        RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, borderRadius, borderRadius);
        SelectObject(hdc, oldBackground);
        SelectObject(hdc, oldBorder);
        DeleteObject(background);
        DeleteObject(border);
    }
};

class Text : public Element {
private:
    std::wstring text = L"";
    int fontSize = 12;
    HFONT font = nullptr;
    COLORREF textColor = RGB(0, 0, 0);
    COLORREF backgroundColor = TRANSPARENT;
    int textAlignment = DT_LEFT;

    int getTextWidth() {
        HDC hdc = GetDC(NULL);
        HGDIOBJ oldFont = SelectObject(hdc, font);
        SIZE textSize;
        GetTextExtentPoint32W(hdc, text.c_str(), text.size(), &textSize);
        SelectObject(hdc, oldFont);
        ReleaseDC(NULL, hdc);
        return textSize.cx + 8;
    }
public:
    std::wstring getText() { return text; }
    Text& setText(const std::wstring& newText) { 
        text = newText; 
        this->setSize({getTextWidth(), fontSize + 8});
        return *this; 
    }
    int getFontSize() { return fontSize; }
    Text& setFontSize(int newFontSize) { 
        fontSize = newFontSize; 
        this->setSize({getTextWidth(), fontSize + 8});
        return *this; 
    }
    HFONT getFont() { return font; }
    Text& setFont(HFONT newFont) { 
        // ! assumes that only font family changes
        font = newFont; 
        this->setSize({getTextWidth(), fontSize + 8});
        return *this; 
    }
    COLORREF getTextColor() { return textColor; }
    Text& setTextColor(COLORREF newTextColor) { textColor = newTextColor; return *this; }
    COLORREF getBackgroundColor() { return backgroundColor; }
    Text& setBackgroundColor(COLORREF newBackgroundColor) { backgroundColor = newBackgroundColor; return *this; }
    int getTextAlignment() { return textAlignment; }
    Text& setTextAlignment(int newTextAlignment) { textAlignment = newTextAlignment; return *this; }
    ~Text() {
        if (font) {
            DeleteObject(font);
            font = nullptr;
        }
    }
    void draw(HDC hdc) override {
        if (backgroundColor == TRANSPARENT) {
            SetBkMode(hdc, TRANSPARENT);
        } else {
            SetBkColor(hdc, backgroundColor);
        }
        SetTextColor(hdc, textColor);
        HFONT oldFont = (HFONT)SelectObject(hdc, font);
        RECT rect = this->getRect();
        DrawTextW(hdc, text.c_str(), -1, &rect, textAlignment);
        SelectObject(hdc, oldFont);
    }
};

class Circle : public Element {
private:
    COLORREF backgroundColor = TRANSPARENT;
    COLORREF borderColor = TRANSPARENT;
    int borderWidth = 0;
public:
    COLORREF getBackgroundColor() { return backgroundColor; }
    COLORREF getBorderColor() { return borderColor; }
    int getBorderWidth() { return borderWidth; }
    Circle& setBackgroundColor(COLORREF newBackgroundColor) { backgroundColor = newBackgroundColor; return *this; }
    Circle& setBorderColor(COLORREF newBorderColor) { borderColor = newBorderColor; return *this; }
    Circle& setBorderWidth(int newBorderWidth) { borderWidth = newBorderWidth; return *this; }
    void draw(HDC hdc) override {
        HBRUSH background = CreateSolidBrush(backgroundColor);
        HPEN border = CreatePen(PS_SOLID, borderWidth, borderColor);
        HGDIOBJ oldBackground = SelectObject(hdc, background);
        HGDIOBJ oldBorder = SelectObject(hdc, border);
        RECT rect = this->getRect();
        Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
        SelectObject(hdc, oldBackground);
        SelectObject(hdc, oldBorder);
        DeleteObject(background);
        DeleteObject(border);
    }
};

class Slider : public Element {
private:
    int value = 0;
    std::shared_ptr<Text> label = std::make_shared<Text>();
    std::shared_ptr<Box> track = std::make_shared<Box>();
    std::shared_ptr<Circle> thumb = std::make_shared<Circle>();
    bool dragging = false;

    void syncThumbPosition() {
        // ! syncs the thumb position to the value
        POINT thumbRange = {
            track->getRect().left + thumb->getSize().width / 2,
            track->getRect().right - thumb->getSize().width / 2
        };
        int thumbOffset = thumbRange.x + (thumbRange.y - thumbRange.x) * value / 100.0f;
        thumb->setPosition({
            thumbOffset - thumb->getSize().width / 2,
            thumb->getPosition().y
        });
    }

    int getRelativeSliderValue(POINT cursorPos) {
        RECT trackRect = track->getRect();
        int relativeValue = (cursorPos.x - trackRect.left) * 100 / (trackRect.right - trackRect.left);
        return std::clamp(relativeValue, 0, 100);
    }
public:
    int getValue() { return value; }
    Slider& setValue(int newValue) {
        value = std::clamp(newValue, 0, 100);
        label->setText(std::to_wstring(value) + L"%");
        return *this;
    }
    Text& getLabel() { return *label; }
    Box& getTrack() { return *track; }
    Circle& getThumb() { return *thumb; }
    void draw(HDC hdc) override {
        syncThumbPosition();
        label->draw(hdc);
        track->draw(hdc);
        thumb->draw(hdc);
    }
    void handler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override {
        switch (msg) {
            case WM_LBUTTONDOWN: {
                POINT cursorPos;
                GetCursorPos(&cursorPos);
                ScreenToClient(hwnd, &cursorPos);
                RECT thumbRect = thumb->getRect();
                if (PtInRect(&thumbRect, cursorPos)) {
                    dragging = true;
                    SetCapture(hwnd);
                }
                break;
            }
            case WM_MOUSEMOVE: {
                if (dragging) {
                    POINT cursorPos;
                    GetCursorPos(&cursorPos);
                    ScreenToClient(hwnd, &cursorPos);
                    setValue(getRelativeSliderValue(cursorPos));
                    RECT thumbRect = thumb->getRect();
                    InvalidateRect(hwnd, &thumbRect, TRUE);
                    RECT textRect = label->getRect();
                    // ! add a little extra padding to avoid cut off
                    textRect.right += 20;
                    InvalidateRect(hwnd, &textRect, TRUE);
                }
                break;
            }
            case WM_LBUTTONUP: {
                if (dragging) {
                    dragging = false;
                    ReleaseCapture();
                }
                break;
            }
        }
    }
};

class Dropdown : public Element {
private:
    std::shared_ptr<Box> selectedContainer = std::make_shared<Box>();
    std::shared_ptr<Text> selectedText = std::make_shared<Text>();

    std::shared_ptr<Box> optionsContainer = std::make_shared<Box>();
    std::vector<std::shared_ptr<Text>> optionElements;
    std::vector<std::wstring> options;
    bool showOptions = false;
    int selectedIndex = 0;
    HFONT font = nullptr;

public:
    Dropdown& setFont(HFONT newFont) {
        font = newFont;
        return *this;
    }
    std::shared_ptr<Box> getSelectedContainer() { return selectedContainer; }
    std::shared_ptr<Text> getSelectedText() { return selectedText; }
    std::shared_ptr<Box> getOptionsContainer() { return optionsContainer; }
    std::vector<std::wstring> getOptions() { return options; }
    Dropdown& setOptions(const std::vector<std::wstring>& newOptions) { 
        options = newOptions;
        optionElements.clear();
        for (size_t i = 0; i < options.size(); i++) {
            std::shared_ptr<Text> optionText = std::make_shared<Text>();
            optionText->setText(options[i]).setFontSize(12).setTextColor(RGB(0, 0, 0));
            optionElements.push_back(optionText);
        }
        return *this;
    }
    int getSelectedIndex() { return selectedIndex; }
    Dropdown& setSelectedIndex(int newIndex) {
        if (newIndex >= 0 && newIndex < static_cast<int>(options.size())) {
            selectedIndex = newIndex;
            selectedText->setText(options[selectedIndex]).centerFromElement(selectedContainer);
        }
        return *this;
    }
    void draw(HDC hdc) override {  
        selectedContainer->draw(hdc);
        selectedText->draw(hdc);
        if (showOptions) {
            optionsContainer->draw(hdc);
            for (size_t i = 0; i < optionElements.size(); i++) {
                optionElements[i]->setFont(font).setPosition({
                    0,
                    optionsContainer->getPosition().y + 8 + static_cast<int>(i) * 36 
                }).horizontalCenterFromElement(optionsContainer);
                optionElements[i]->draw(hdc);
            }
        }
    }
    void handler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override {
        if (msg == WM_LBUTTONDOWN) {
            POINT cursorPos;
            GetCursorPos(&cursorPos);
            ScreenToClient(hwnd, &cursorPos);
            RECT selectedRect = selectedContainer->getRect();
            RECT optionsRect = optionsContainer->getRect();
            if (PtInRect(&selectedRect, cursorPos)) {
                showOptions = !showOptions;
                InvalidateRect(hwnd, &optionsRect, TRUE);
            }
            if (PtInRect(&optionsRect, cursorPos) && showOptions) {
                int optionHeight = 36;
                int relativeY = cursorPos.y - optionsRect.top;
                int clickedIndex = relativeY / optionHeight;
                if (clickedIndex >= 0 && clickedIndex < static_cast<int>(options.size())) {
                    setSelectedIndex(clickedIndex);
                    showOptions = false;
                    InvalidateRect(hwnd, &selectedRect, TRUE);
                    InvalidateRect(hwnd, &optionsRect, TRUE);
                }
            }
        }
    }
};

class TextInput : public Element {
private:
    std::shared_ptr<Text> label = std::make_shared<Text>();
    std::shared_ptr<Box> container = std::make_shared<Box>();
    bool focus = false;
    std::wstring validCharacters = L"";
    int minLength = 0;
    int maxLength = 50;
    std::function<void()> onSubmit = nullptr;

    bool isValidChar(wchar_t inputChar) {
        return (validCharacters.find(inputChar) != std::wstring::npos);
    }
public:
    std::shared_ptr<Text> getLabel() { return label; }
    std::shared_ptr<Box> getContainer() { return container; }
    TextInput& setFocus(bool newFocus) { focus = newFocus; return *this; }
    bool isFocused() { return focus; }
    TextInput& setValidCharacters(std::wstring newValidCharacters) { validCharacters = newValidCharacters; return *this; }
    TextInput& setMaxLength(int newMaxLength) { maxLength = newMaxLength; return *this; }
    TextInput& setMinLength(int newMinLength) { minLength = newMinLength; return *this; }
    TextInput& setOnSubmit(std::function<void()> newOnSubmit) { onSubmit = newOnSubmit; return *this; }
    void draw(HDC hdc) override {  
        container->draw(hdc);
        label->draw(hdc);
    }
    void handler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override {
        switch (msg) {
            case WM_LBUTTONDOWN: {
                POINT cursorPos;
                GetCursorPos(&cursorPos);
                ScreenToClient(hwnd, &cursorPos);
                RECT containerRect = container->getRect();
                if (PtInRect(&containerRect, cursorPos)) {
                    if (focus) {
                        container->setBorderColor(RGB(0, 0, 0));
                    } else {
                        container->setBorderColor(RGB(0, 0, 255));
                    }
                    focus = !focus;
                }
                InvalidateRect(hwnd, &containerRect, TRUE);
                break;
            }
            case WM_CHAR: {
                if (focus) {
                    switch (wParam) {
                        case VK_RETURN:
                        case VK_ESCAPE: {
                            focus = false;
                            container->setBorderColor(RGB(0, 0, 0));
                            if (onSubmit) { onSubmit(); }
                            RECT tempRect = container->getRect();
                            InvalidateRect(hwnd, &tempRect, TRUE);
                            break;
                        }
                        case VK_BACK: {
                            std::wstring currentText = label->getText();
                            if (currentText.length() > static_cast<size_t>(minLength)) {
                                currentText.pop_back();
                                label->setText(currentText);
                                RECT tempRect = getRect();
                                InvalidateRect(hwnd, &tempRect, TRUE);
                            }
                            break;
                        }
                        default: {
                            std::wstring currentText = label->getText();
                            if (isValidChar(static_cast<wchar_t>(wParam)) && currentText.length() < static_cast<size_t>(maxLength)) {
                                label->setText(currentText + static_cast<wchar_t>(wParam));
                                RECT tempRect = getRect();
                                InvalidateRect(hwnd, &tempRect, TRUE);
                            }
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
};
