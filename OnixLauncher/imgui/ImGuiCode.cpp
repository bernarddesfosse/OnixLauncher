#ifndef NDEBUG
#include <windows.h>
#include <fstream>
#include "../json.hpp"
namespace json = nlohmann;

#include "imgui.h"
#include "../UIManager.h"
#include "../resource.h"
#include "../Logger.h"

extern UIManager* mainWindow;
extern UIManager* settingsWindow;
extern UIManager* lastMsgBox;

void RenderImGuiFrame() {
    //ImGui::Begin("RandomDebug");
    //ImGui::Text("elapsedTimeTotal: %f", elapsedTimeTotal);
    //ImGui::End();
    UIManager* window = lastMsgBox;
    if (!window)
        window = settingsWindow;

    if (!window) return;
    ImGui::Begin("Elements");
    

    int currIdentifiering = 0;
    if (window)
    for (UIElement* elem : window->elements) {
        currIdentifiering++;
        if (ImGui::TreeNode(elem->name.c_str()[0] == 0 ? "##" : elem->name.c_str())) {
            ImGui::PushID(currIdentifiering);
            ImGui::SliderFloat2("Position", &elem->posX, 0.f, 1000.f, "%.3f", 1.f);
            ImGui::SliderFloat2("Size", &elem->sizeX, 0.f, 1000.f, "%.3f", 1.f);
            if (elem->name.capacity() < 256)
                elem->name.reserve(256);
            ImGui::InputText("Name", elem->name.data(), 256);
            ImGui::Checkbox("Visible", &elem->visible);

            ImGui::LabelText("Type", std::to_string(elem->type).c_str());

            if (elem->type == UIElement::Type::Image) {
                ImageElement* ielem = (ImageElement*)elem;
                ImGui::SliderInt("Resource", &ielem->resourceIndex, -1, 16);
            }
            if (elem->type == UIElement::Type::Text) {
                TextElement* telem = (TextElement*)elem;
                ImGui::SliderFloat("Font Size", &telem->fontSize, 1.f, 169, "%.3f", 1);
                ImGui::Checkbox("Bold", &telem->bold);
                if (telem->text.capacity() < 256)
                    telem->text.reserve(256);
                ImGui::ColorEdit4("Color", (float*)&telem->color, ImGuiColorEditFlags_::ImGuiColorEditFlags_Float | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
                ImGui::InputText("Text", telem->text.data(), 256);
            }
            if (elem->type == UIElement::Type::RoundRect) {
                RoundRectElement* rrelem = (RoundRectElement*)elem;
                ImGui::SliderFloat("Roundness", &rrelem->roundness, 1.f, 169, "%.3f", 1);
                ImGui::ColorEdit4("Color", (float*)&rrelem->color, ImGuiColorEditFlags_::ImGuiColorEditFlags_Float | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
            }
            ImGui::PopID();
            ImGui::TreePop();
        }
    }


    static char InputTextBuffer[256] = "Test";
    ImGui::InputText("Name: ", InputTextBuffer, 256);
    if (ImGui::Button("AddImageElement")) {
        ImageElement& imelem = (ImageElement&)window->addUiElement(new ImageElement);
        imelem.name = InputTextBuffer;
    }
    ImGui::SameLine();
    if (ImGui::Button("AddTextElement")) {
        TextElement& imelem = (TextElement&)window->addUiElement(new TextElement);
        imelem.name = InputTextBuffer;
    }
    ImGui::SameLine();
    if (ImGui::Button("AddRoundRectElement")) {
        RoundRectElement& imelem = (RoundRectElement&)window->addUiElement(new RoundRectElement);
        imelem.name = InputTextBuffer;
    }

    if (ImGui::Button("Save")) {
            Logger::logf("Saving at: %s", Logger::Level::Info, "Layout.json");
            Logger::log("Saving layout...");
            std::ofstream out("Layout.json");
            if (out.is_open()) {
                json::json root;
                for (UIElement* elem : window->elements) {
                    json::json position;
                    position["x"] = elem->posX;
                    position["y"] = elem->posY;
                    json::json size;
                    size["x"] = elem->sizeX;
                    size["y"] = elem->sizeY;

                    json::json element;
                    element["position"] = position;
                    element["size"] = size;
                    element["visible"] = elem->visible;
                    element["name"] = elem->name.c_str();
                    element["type"] = elem->type;

                    if (elem->type == UIElement::Type::Image) {
                        ImageElement* ielem = (ImageElement*)elem;
                        element["resource"] = ielem->resourceIndex;
                    }
                    if (elem->type == UIElement::Type::Text) {
                        TextElement* telem = (TextElement*)elem;
                        element["font_size"] = telem->fontSize;
                        element["bold"] = telem->bold;
                        element["text"] = telem->text.c_str();
                        json::json color;
                        color["r"] = telem->color.r;
                        color["g"] = telem->color.g;
                        color["b"] = telem->color.b;
                        color["a"] = telem->color.a;
                        element["color"] = color;
                    }
                    root.push_back(element);
                }

                out << std::setw(4) << root;
                out.close();
                Logger::log("Layout was saved!", Logger::Level::Info);
            } else
                Logger::log("Could not save", Logger::Level::Error);
    }
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        {
            window->elementsLock.lock();
            std::vector<UIElement*> elems = std::move(window->elements);
            for (auto* elem : elems)
                delete elem;
            window->elementsLock.unlock();
        }

        std::ifstream in("Layout.json");
        if (in.is_open()) {
            json::json root = json::json::parse(in);
            for (auto& elem : root) {
                UIElement* res = 0;
                UIElement::Type type = elem["type"];
                switch (type) {
                case UIElement::Type::Base:
                    res = new UIElement;
                    break;
                case UIElement::Type::Image: {
                    ImageElement* img = new ImageElement;
                    res = img;
                    img->resourceIndex = elem["resource"];
                    break;
                }
                case UIElement::Type::Text: {
                    TextElement* txt = new TextElement;
                    res = txt;
                    txt->fontSize = elem["font_size"];
                    txt->bold = elem["bold"];
                    txt->text = elem["text"];
                    txt->color.r = elem["color"]["r"];
                    txt->color.g = elem["color"]["g"];
                    txt->color.b = elem["color"]["b"];
                    txt->color.a = elem["color"]["a"];
                    break;
                }
                case UIElement::Type::RoundRect: {
                    RoundRectElement* rre = new RoundRectElement;
                    res = rre;
                    rre->roundness = elem["roundness"];
                    rre->color.r = elem["color"]["r"];
                    rre->color.g = elem["color"]["g"];
                    rre->color.b = elem["color"]["b"];
                    rre->color.a = elem["color"]["a"];
                    break;
                }
                }

                res->posX = elem["position"]["x"];
                res->posY = elem["position"]["y"];
                res->sizeX = elem["size"]["x"];
                res->sizeY = elem["size"]["y"];
                res->name = elem["name"];
                res->visible = elem["visible"];
                window->addUiElement(res);
            }
        }
        else
            Logger::log("Could not load!", Logger::Level::Error);
    }
    ImGui::SameLine();
    if (ImGui::Button("ExportAnimtion")) {
        Logger::logf("Saving at: %s", Logger::Level::Info, "Animation.txt");
        Logger::log("Saving layout...");
        std::ofstream out("Animation.txt");
        if (out.is_open()) {
            out << "{\nLayoutAnimation& animation = DEFAULT;\n\n";
            for (UIElement* elem : window->elements) {
                out << "\t{\n\tLayoutAnimation::Data animationData;\n";
                out << "\t\tanimationData.elementName = \"" << elem->name.c_str() << "\";\n";
                out << "\t\tanimationData.posX = (float)" << elem->posX << ";\n";
                out << "\t\tanimationData.posY = (float)" << elem->posY << ";\n";
                out << "\t\tanimationData.sizeX = (float)" << elem->sizeX << ";\n";
                out << "\t\tanimationData.sizeY = (float)" << elem->sizeY << ";\n";

                if (elem->type == UIElement::Type::Text) {
                    TextElement* telem = (TextElement*)elem;
                    out << "\t\tanimationData.fontSize = (float)" << telem->fontSize << ";\n";
                } else if (elem->type == UIElement::Type::RoundRect) {
                    RoundRectElement* telem = (RoundRectElement*)elem;
                    out << "\t\tanimationData.fontSize = (float)" << telem->roundness << ";\n";
                }
                out << "\n\t\tanimation.actions.emplace_back(animationData);\n";
                out << "\t}\n";
            }
            out << "}";
            out.close();
            Logger::log("Animation was saved!", Logger::Level::Info);
        }
        else
            Logger::log("Could not save", Logger::Level::Error);
    }
    ImGui::SameLine();
    if (ImGui::Button("ExportWindowCreation")) {
        Logger::logf("Saving at: %s", Logger::Level::Info, "Layout.txt");
        Logger::log("Saving layout...");
        std::ofstream out("Layout.txt");
        if (out.is_open()) {
            json::json root;
            for (UIElement* elem : window->elements) {
                out << "{ //" << elem->name.c_str() << "\n";
                switch (elem->type) {
                case UIElement::Type::Text:
                    out << "TextElement& element = (TextElement&)window->addUiElement(new TextElement);\n";
                    break;
                case UIElement::Type::Image:
                    out << "ImageElement& element = (ImageElement&)window->addUiElement(new ImageElement);\n";
                    break;
                case UIElement::Type::RoundRect:
                    out << "RoundRectElement& element = (RoundRectElement&)window->addUiElement(new RoundRectElement);\n";
                    break;
                default:
                    out << "UIElement& element = (UIElement&)window->addUiElement(new UIElement);\n";
                }

                   
                switch (elem->type) {
                case UIElement::Type::Text: {
                    TextElement* element = (TextElement*)elem;
                    out << "\telement.fontSize = (float)" << element->fontSize << ";\n";
                    out << "\telement.bold = " << (element->bold ? "true" : "false") << ";\n";
                    out << "\telement.color = { (float)" << element->color.r << ", (float)" << element->color.g << ", (float)" << element->color.b << ", (float)" << element->color.a << " };\n";
                    out << "\telement.text = \"" << element->text.c_str() << "\";\n";
                    break; }
                case UIElement::Type::Image: {
                    ImageElement* element = (ImageElement*)elem;
                    out << "\telement.resourceIndex = " << element->resourceIndex << ";\n";
                    break; }
                case UIElement::Type::RoundRect: {
                    RoundRectElement* element = (RoundRectElement*)elem;
                    out << "\telement.roundness = (float)" << element->roundness << ";\n";
                    out << "\telement.color = { (float)" << element->color.r << ", (float)" << element->color.g << ", (float)" << element->color.b << ", (float)" << element->color.a << " };\n";
                    break; }
                }

                out << "\telement.name = \"" << elem->name.c_str() << "\";\n";
                out << "\telement.posX = (float)" << elem->posX << ";\n";
                out << "\telement.posY = (float)" << elem->posY << ";\n";
                out << "\telement.sizeX = (float)" << elem->sizeX << ";\n";
                out << "\telement.sizeY = (float)" << elem->sizeY << ";\n";

                out << "}\n\n";
            }

            out.close();
            Logger::log("Animation was saved!", Logger::Level::Info);
        }
        else
            Logger::log("Could not save", Logger::Level::Error);
    }
    ImGui::End();
}

#endif
