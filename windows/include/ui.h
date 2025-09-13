// custom ui library for keylogger
// ! use absolute coordinates relative to the window client to avoid headaches
#pragma once

#include <windows.h>
#include <vector>
#include <memory>
#include <iostream>

enum FlexDirection {
    HORIZONTAL,
    VERTICAL
};

class Element {
public:
    int x, y, width, height;
    std::vector<std::unique_ptr<Element>> children;
    Element(int x = 0, int y = 0, int width = 0, int height = 0) : x(x), y(y), width(width), height(height) {}
    virtual void draw(HDC hdc) {
        for (size_t i = 0; i < children.size(); i++) {
            children[i]->draw(hdc);
        }
    }
};

class FlexContainer : public Element {
public:
    FlexDirection direction;
    int gapX, gapY;
    FlexContainer(int x = 0, int y = 0, int width = 0, int height = 0, FlexDirection direction = HORIZONTAL, int gapX = 0, int gapY = 0) : Element(x, y, width, height), direction(direction), gapX(gapX), gapY(gapY) {}
    void draw(HDC hdc) override {
        int currentX = x;
        int currentY = y;
        for (size_t i = 0; i < children.size(); i++) {
            children[i]->x = currentX;
            children[i]->y = currentY;
            children[i]->draw(hdc);
            if (direction == HORIZONTAL) {
                currentX += children[i]->width + gapX;
            } else {
                MessageBox(NULL, (std::to_wstring(children[i]->height) + L" " + std::to_wstring(currentY)).c_str(), L"Height", MB_OK);
                currentY += children[i]->height + gapY;
            }
        }
    }
};

class PaddingContainer : public Element {
public: 
    int px, py;
    PaddingContainer(int x = 0, int y = 0, int width = 0, int height = 0, int px = 0, int py = 0) : Element(x, y, width, height), px(px), py(py) {
        this->x += px;
        this->y += py;
        this->width -= 2 * px;
        this->height -= 2 * py;
    }
};

class Box : public Element {
public:
    COLORREF color;
    int borderRadius;
    Box(int x = 0, int y = 0, int width = 0, int height = 0, COLORREF color = RGB(255, 255, 255), int borderRadius = 0) : Element(x, y, width, height), color(color), borderRadius(borderRadius) {}
    void draw(HDC hdc) override {
        HBRUSH brush = CreateSolidBrush(color);
        RECT rect = {x, y, width, height};
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);
    }
};

// ! font set by font manager
class Text : public Element {
public:
    int fontSize;
    std::wstring text;
    COLORREF textColor;
    COLORREF bgColor;
    Text(int x = 0, int y = 0, int width = 0, int height = 0, std::wstring text = L"", int fontSize = 12, COLORREF textColor = RGB(0, 0, 0), COLORREF bgColor = TRANSPARENT) 
        : Element(x, y, width, height), text(text), fontSize(fontSize), textColor(textColor), bgColor(bgColor) {}
    void draw(HDC hdc) override {
        SetBkMode(hdc, bgColor);
        SetTextColor(hdc, textColor);
        RECT rect = {x, y, width, y+height};
        DrawTextW(hdc, text.c_str(), -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
};

class Slider : public Element {
public:
    int value;
    Slider(int x = 0, int y = 0, int width = 0, int height = 0, int value = 0) : Element(x, y, width, height), value(value) {}
    void draw(HDC hdc) override {
        HBRUSH trackBrush = CreateSolidBrush(RGB(200, 200, 200));
        RECT trackRect = {x, y+(height-height/4)/2 , x + width, y + height / 4};
        FillRect(hdc, &trackRect, trackBrush);
        DeleteObject(trackBrush);

        HBRUSH thumbBrush = CreateSolidBrush(RGB(0, 0, 0));
        HGDIOBJ oldBrush = (HGDIOBJ)SelectObject(hdc, thumbBrush);
        Ellipse(hdc, x, y, x+height, y+height);
        SelectObject(hdc, oldBrush);
        DeleteObject(thumbBrush);
    }
};

class KeyloggerUI {
public:
    int windowWidth, windowHeight;
    std::unique_ptr<Element> root;
    KeyloggerUI(): windowWidth(0), windowHeight(0) {}
    KeyloggerUI(int windowWidth, int windowHeight): windowWidth(windowWidth), windowHeight(windowHeight) {
        root = std::make_unique<Element>(0, 0, windowWidth, windowHeight);
        std::unique_ptr<Element> paddingContainer = std::make_unique<PaddingContainer>(root->x, root->y, root->width, root->height, 16, 16);
        std::unique_ptr<Element> flexContainer = std::make_unique<FlexContainer>(paddingContainer->x, paddingContainer->y, paddingContainer->width, paddingContainer->height, VERTICAL, 0, 16);
        std::unique_ptr<Element> display = std::make_unique<Box>(0, 0, flexContainer->width, 80, RGB(200, 200, 200), 8);
        std::unique_ptr<Element> sectionText1 = std::make_unique<Text>(0, 0, flexContainer->width, 24, L"Transparency", 16, RGB(0, 0, 0), TRANSPARENT);
        std::unique_ptr<Element> slider = std::make_unique<Slider>(0, 0, 360, 16, 50);
        std::unique_ptr<Element> sectionText2 = std::make_unique<Text>(0, 0, flexContainer->width, 24, L"Font", 16, RGB(0, 0, 0), TRANSPARENT);
        flexContainer->children.push_back(std::move(display));
        flexContainer->children.push_back(std::move(sectionText1));
        flexContainer->children.push_back(std::move(slider));
        flexContainer->children.push_back(std::move(sectionText2));
        paddingContainer->children.push_back(std::move(flexContainer));
        root->children.push_back(std::move(paddingContainer));
    }
    void draw(HDC hdc)
    {
        root->draw(hdc);
    }
};