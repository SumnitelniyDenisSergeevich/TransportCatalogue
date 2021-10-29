#include "svg.h"

#include <iostream> // delete

namespace svg {

    using namespace std::literals;

    std::ostream& operator<<(std::ostream& out, const StrokeLineCap& linecap) {
        switch (linecap) {
        case StrokeLineCap::kButt:
            out << "butt"s;
            break;
        case StrokeLineCap::kRound:
            out << "round"s;
            break;
        case StrokeLineCap::kSquare:
            out << "square"s;
            break;
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& linejoin) {
        switch (linejoin) {
        case StrokeLineJoin::kArcs:
            out << "arcs"s;
            break;
        case StrokeLineJoin::kBevel:
            out << "bevel"s;
            break;
        case StrokeLineJoin::kMiter:
            out << "miter"s;
            break;
        case StrokeLineJoin::kMiterClip:
            out << "miter-clip"s;
            break;
        case StrokeLineJoin::kRound:
            out << "round"s;
            break;
        }
        return out;
    }

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ---------- Polyline ------------------

    Polyline& Polyline::AddPoint(Point point) {
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        bool flag = false;
        auto& out = context.out;
        out << "<polyline points=\""sv;
        for (const auto& point : points_) {
            if (flag) {
                out << ' ';
            }
            out << point.x << ',' << point.y;
            flag = true;
        }
        out << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ---------- Text ------------------
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& Text::SetPosition(Point pos) {
        position_ = pos;
        return *this;
    }

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    // Задаёт размеры шрифта (атрибут font-size)
    Text& Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }

    // Задаёт название шрифта (атрибут font-family)
    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = font_family;
        return *this;
    }

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = font_weight;
        return *this;
    }

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& Text::SetData(std::string data) {
        data_ = data;
        RepairData();
        return *this;
    }

    void Text::RepairData() {
        if (auto iter = data_.find('&'); iter != data_.npos) {
            for (; iter != data_.size(); ++iter) {
                if (data_[iter] == '&') {
                    data_.insert(iter + 1, "amp;"sv);
                    iter += 4;
                }
            }
        }
        Ecranirovanie_Simbols('"', "&quot;"s);
        Ecranirovanie_Simbols('\'', "&apos;"s);
        Ecranirovanie_Simbols('<', "&lt;"s);
        Ecranirovanie_Simbols('>', "&gt;"s);
    }
    void Text::Ecranirovanie_Simbols(char ch, const std::string& ins) {
        auto iter = data_.find(ch);
        while (iter != data_.npos) {
            data_.erase(iter, 1);
            data_.insert(iter, ins);
            iter = data_.find(ch);
        }
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<text"s;
        RenderAttrs(out);
        out << " x=\""sv << position_.x << "\" y=\""sv << position_.y <<
            "\" dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y <<
            "\" font-size=\""sv << font_size_ << "\""sv;
        if (font_family_.has_value()) {
            out << " font-family=\""sv << font_family_.value() << "\""sv;
        }
        if (font_weight_.has_value()) {
            out << " font-weight=\""sv << font_weight_.value() << "\""sv;
        }
        out << ">"sv << data_ << "</text>"sv;
    }

    // ---------- Document ------------------

    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        renders_.push_back(std::move(obj));
    }

    // Выводит в ostream svg-представление документа
    void Document::Render(std::ostream& out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        for (auto& render : renders_) {
            render->Render({ out,2,2 });
        }
        out << "</svg>"sv;
    }
}  // namespace svg