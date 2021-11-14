#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <variant>

namespace svg {
    struct Point {
        Point() = default;
        Point(double x, double y)
            : x(x)
            , y(y) {
        }
        double x = 0;
        double y = 0;
    };
    struct Rgb {
        Rgb(uint8_t red_, uint8_t green_, uint8_t blue_) :red(red_), green(green_), blue(blue_) {

        }
        Rgb() = default;
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };

    struct Rgba {
        Rgba(uint8_t red_, uint8_t green_, uint8_t blue_, double opacity_) :red(red_), green(green_), blue(blue_), opacity(opacity_) {

        }
        Rgba() = default;
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        double opacity = 1.0;
    };
    using Color = std::variant<std::string, Rgb, Rgba>;

    // Объявив в заголовочном файле константу со спецификатором inline,
    // мы сделаем так, что она будет одной на все единицы трансляции,
    // которые подключают этот заголовок.
    // В противном случае каждая единица трансляции будет использовать свою копию этой константы
    inline const Color NoneColor{ "none" };

    enum class StrokeLineCap {
        kButt = 0,
        kRound = 1,
        kSquare = 2
    };
    std::ostream& operator<<(std::ostream& out, const StrokeLineCap& linecap);
    enum class StrokeLineJoin {
        kArcs = 0,
        kBevel = 1,
        kMiter = 2,
        kMiterClip = 3,
        kRound = 4
    };
    std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& linejoin);


    //------------------------RenderContext---------------------
    /*
     * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
     * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
     */
    struct RenderContext {
        RenderContext(std::ostream& out)
            : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out)
            , indent_step(indent_step)
            , indent(indent) {
        }

        RenderContext Indented() const {
            return { out, indent_step, indent + indent_step };
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };
    //-----------------------"<<interface>>"------------------- есть шаблонный метод
    //-----------------------ObjectContainer -------------------
    class Object;
    class ObjectContainer {
    public:
        virtual ~ObjectContainer() = default;
        template <typename T>
        void Add(T obj) {
            AddPtr(std::make_unique<T>(std::move(obj)));
        }
        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    };

    //-----------------------<<interface>> -------------------
    //-----------------------Drawable -------------------
    class Drawable {
    public:
        virtual ~Drawable() = default;
        virtual void Draw(ObjectContainer& container) const = 0;
    };
    //------------------------Object---------------------
    /*
     * Абстрактный базовый класс Object служит для унифицированного хранения
     * конкретных тегов SVG-документа
     * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
     */
    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    //--------------------------PathProps-----------
    using namespace std::literals;
    struct ColorPrinter {
        std::ostream& out;
        void operator()(std::string color) {
            out << color;
        }
        void operator()(Rgb color) {//hz
            out << "rgb(" << (int)color.red << ","s << (int)color.green << ","s << (int)color.blue << ")"s;
        }
        void operator()(Rgba color) {
            out << "rgba(" << (int)color.red << ","s << (int)color.green << ","s << (int)color.blue << ","s << color.opacity << ")"s;
        }
    };

    template <typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(Color color) {
            fill_color_ = color;
            return AsOwner();
        }

        Owner& SetStrokeColor(Color color) {
            stroke_color_ = color;
            return AsOwner();
        }

        Owner& SetStrokeWidth(double width) {
            width_ = std::move(width);
            return AsOwner();
        }

        Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
            line_cap_ = std::move(line_cap);
            return AsOwner();
        }

        Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
            line_join_ = std::move(line_join);
            return AsOwner();
        }
    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const {
            using namespace std::literals;

            if (fill_color_) {
                out << " fill=\""sv;
                std::visit(ColorPrinter{ out }, *fill_color_);
                out << "\""sv;
            }
            if (stroke_color_) {
                out << " stroke=\""sv;
                std::visit(ColorPrinter{ out }, *stroke_color_);
                out << "\""sv;
            }
            if (width_) {
                out << " stroke-width=\""sv << *width_ << "\""sv;
            }
            if (line_cap_) {
                out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
            }
            if (line_join_) {
                out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
            }
        }

    private:
        Owner& AsOwner() {
            // static_cast безопасно преобразует *this к Owner&,
            // если класс Owner — наследник PathProps
            return static_cast<Owner&>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> width_;
        std::optional<StrokeLineCap> line_cap_;
        std::optional<StrokeLineJoin> line_join_;

    };



    //------------------------Circle---------------------
    /*
     * Класс Circle моделирует элемент <circle> для отображения круга
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
     */
    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };
    //------------------------Polyline---------------------
    /*
     * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
     */
    class Polyline final : public Object, public PathProps<Polyline> {
    public:
        Polyline& AddPoint(Point point);
    private:
        void RenderObject(const RenderContext& context) const override;

        std::vector<Point> points_ = {};
    };
    //------------------------Text---------------------
    /*
     * Класс Text моделирует элемент <text> для отображения текста
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
     */
    class Text final : public Object, public PathProps<Text> {
    public:
        // Задаёт координаты опорной точки (атрибуты x и y)
        Text& SetPosition(Point pos);

        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text& SetOffset(Point offset);

        // Задаёт размеры шрифта (атрибут font-size)
        Text& SetFontSize(uint32_t size);

        // Задаёт название шрифта (атрибут font-family)
        Text& SetFontFamily(std::string font_family);

        // Задаёт толщину шрифта (атрибут font-weight)
        Text& SetFontWeight(std::string font_weight);

        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text& SetData(std::string data);
    private:
        void RenderObject(const RenderContext& context) const override;

        void RepairData();
        void Ecranirovanie_Simbols(char ch, const std::string& ins);

        Point position_;
        Point offset_;
        uint32_t font_size_ = 1;
        std::optional<std::string> font_family_;
        std::optional<std::string> font_weight_;
        std::string data_ = {};
    };
    //------------------------Document---------------------
    class Document : public ObjectContainer {
    public:

        template <typename Obj>
        void Add(Obj obj) {
            renders_.push_back(std::make_unique<Obj>(std::move(obj)));
        }
        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::unique_ptr<Object>&& obj) override;

        // Выводит в ostream svg-представление документа
        void Render(std::ostream& out) const;
    private:
        std::vector<std::unique_ptr<Object>> renders_;
    };

}  // namespace svg