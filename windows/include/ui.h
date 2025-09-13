// custom ui library for keylogger
#pragma once

#include <windows.h>
#include <vector>
#include <memory>
#include <iostream>
#include "font_manager.h"


enum FlexDirection {
    HORIZONTAL,
    VERTICAL
};

class Element {
public:
    RECT rect = {0, 0, 0, 0};
    std::vector<std::shared_ptr<Element>> children;
    Element(RECT rect): rect(rect) {}
    virtual void draw(HDC hdc) {
        for (size_t i = 0; i < children.size(); i++) {
            children[i]->draw(hdc);
        }
    }
};

class PaddingContainer : public Element {
public:
    int paddingX, paddingY;
    PaddingContainer(RECT rect, int paddingX, int paddingY) : Element(rect), paddingX(paddingX), paddingY(paddingY) {
        this->rect.left += paddingX;
        this->rect.top += paddingY;
        this->rect.right -= paddingX*2;
        this->rect.bottom -= paddingY*2;
    }
};

class FlexContainer : public Element {
public:
    FlexDirection direction;
    int gapX, gapY;
    FlexContainer(RECT rect, FlexDirection direction, int gapX, int gapY) : Element(rect), direction(direction), gapX(gapX), gapY(gapY) {}
    void draw(HDC hdc) override {
        for (size_t i = 0; i < children.size(); i++) {
            if (i > 0 &&  direction == HORIZONTAL) {
                int childWidth = children[i]->rect.right - children[i]->rect.left;
                children[i]->rect.left = children[i-1]->rect.right + gapX;
                children[i]->rect.right = children[i]->rect.left + childWidth;
            } else if (i > 0 && direction == VERTICAL) {
                int childHeight = children[i]->rect.bottom - children[i]->rect.top;
                children[i]->rect.top = children[i-1]->rect.bottom + gapY;
                children[i]->rect.bottom = children[i]->rect.top + childHeight;
            }
            // MessageBox(NULL, std::to_wstring(children[i]->rect.top).c_str(), L"Debug", MB_OK);
            // MessageBox(NULL, std::to_wstring(children[i]->rect.bottom).c_str(), L"Debug", MB_OK);
            children[i]->draw(hdc);
        }
    }
};

class Box : public Element {
public:
    COLORREF color;
    int borderRadius;
    Box(RECT rect, COLORREF color = RGB(0, 0, 0), int borderRadius = 0) : Element(rect), color(color), borderRadius(borderRadius) {}
    void draw(HDC hdc) override {
        HBRUSH brush = CreateSolidBrush(color);
        HPEN pen = CreatePen(PS_NULL, 0, TRANSPARENT);
        HGDIOBJ oldBrush = SelectObject(hdc, brush);
        HGDIOBJ oldPen = SelectObject(hdc, pen);
        RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, borderRadius, borderRadius);
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(brush);
        DeleteObject(pen);
    }
};

class Text : public Element {
public:
    std::wstring text;
    int fontSize;
    COLORREF textColor;
    COLORREF bgColor;
    HFONT font;
    Text(RECT rect, const std::wstring& text, int fontSize, COLORREF textColor, COLORREF bgColor, HFONT font) : Element(rect), text(text), fontSize(fontSize), textColor(textColor), bgColor(bgColor), font(font) {}
    ~Text() {
        if (font) {
            DeleteObject(font);
            font = nullptr;
        }
    }
    void draw(HDC hdc) override {
        SetBkMode(hdc, bgColor);
        SetTextColor(hdc, textColor);
        HFONT oldFont = (HFONT)SelectObject(hdc, font);
        DrawTextW(hdc, text.c_str(), -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, oldFont);
    }
};

// class Slider : public Element {
// public:
//     int value;
//     Slider(int x = 0, int y = 0, int width = 0, int height = 0, int value = 0) : Element(x, y, width, height), value(value) {}
//     void draw(HDC hdc) override {
//         HBRUSH trackBrush = CreateSolidBrush(RGB(200, 200, 200));
//         RECT trackRect = {x, y+(height-height/4)/2 , x + width, y + height / 4};
//         FillRect(hdc, &trackRect, trackBrush);
//         DeleteObject(trackBrush);

//         HBRUSH thumbBrush = CreateSolidBrush(RGB(0, 0, 0));
//         HGDIOBJ oldBrush = (HGDIOBJ)SelectObject(hdc, thumbBrush);
//         Ellipse(hdc, x, y, x+height, y+height);
//         SelectObject(hdc, oldBrush);
//         DeleteObject(thumbBrush);
//     }
// };

class KeyloggerUI {
public:
    int windowWidth, windowHeight;
    std::shared_ptr<Element> root;
    std::vector<std::wstring> sections {L"Transparency", L"Font", L"Background Color", L"Fade Duration", L"Position"};
    KeyloggerUI(): windowWidth(0), windowHeight(0) {}
    KeyloggerUI(int windowWidth, int windowHeight, HFONT font): windowWidth(windowWidth), windowHeight(windowHeight) {
        root = std::make_shared<Element>(RECT{0, 0, windowWidth, windowHeight});
        std::shared_ptr<Element> background = std::make_shared<Box>(RECT{0, 0, windowWidth, windowHeight}, RGB(24, 24, 37), 0);
        root->children.push_back(background);
        std::shared_ptr<Element> paddingContainer = std::make_shared<PaddingContainer>(RECT{0, 0, windowWidth, windowHeight}, 16, 16);
        root->children.push_back(paddingContainer);
        std::shared_ptr<Element> flexContainer = std::make_shared<FlexContainer>(paddingContainer->rect, VERTICAL, 20, 20);
        paddingContainer->children.push_back(flexContainer);
        std::shared_ptr<Element> display = std::make_shared<Box>(RECT{
            flexContainer->rect.left, 
            flexContainer->rect.top, 
            flexContainer->rect.right, 
            flexContainer->rect.top + 80
        }, RGB(0, 0, 0), 8);
        flexContainer->children.push_back(display);
        for (std::wstring section : sections) {
            std::shared_ptr<Element> sectionText = std::make_shared<Text>(RECT{
                flexContainer->rect.left,
                flexContainer->rect.top, 
                flexContainer->rect.right, 
                flexContainer->rect.top+24
            }, section, 16, RGB(255, 255, 255), TRANSPARENT, font);
            flexContainer->children.push_back(sectionText);
        }
    }
    void draw(HDC hdc)
    {
        root->draw(hdc);
    }
};