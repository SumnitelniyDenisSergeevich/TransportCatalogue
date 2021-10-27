#include "json_reader.h"
#include "request_handler.h"
#include "map_render.h"

using namespace json;
using namespace transport_catalogue;
using namespace renderer;

using namespace std;
#include <string>
#include <istream>

#include <sstream>
#include <iostream>

#include <fstream>



int main() {
    ifstream in("s10_final_opentest_3.json"s);
    ofstream out("result.json"s);

    TransportCatalogue ob1;
    MapRender ob;
    JsonReader json_reader(in);

    RequestHandler request_handler(ob1, ob, json_reader.GetRequests());
    /*auto iter = doc.GetRoot().AsMap().at("render_settings"s);
    render::FillRenderSettings(iter, ob);*/
    //ob.Print();

    json_reader.FillCatalogue(ob1);
    request_handler.FillRenderSettings();
    request_handler.ProcessRequests(out);

    system("pause");
}

//SVG
//#define _USE_MATH_DEFINES
//#include "svg.h"
//
//#include <cmath>
//#include <fstream>
//
//using namespace std::literals;
//using namespace svg;
//
//namespace {
//
//    Polyline CreateStar(Point center, double outer_rad, double inner_rad, int num_rays) {
//        Polyline polyline;
//        for (int i = 0; i <= num_rays; ++i) {
//            double angle = 2 * M_PI * (i % num_rays) / num_rays;
//            polyline.AddPoint({ center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle) });
//            if (i == num_rays) {
//                break;
//            }
//            angle += M_PI / num_rays;
//            polyline.AddPoint({ center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle) });
//
//        }
//        return polyline;
//    }
//
//}  // namespace
//
//namespace shapes {
//
//    class Triangle final : public svg::Drawable {
//    public:
//        Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
//            : p1_(p1)
//            , p2_(p2)
//            , p3_(p3) {
//        }
//
//        // Реализует метод Draw интерфейса svg::Drawable
//        void Draw(svg::ObjectContainer& container) const override {
//            container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
//        }
//
//    private:
//        svg::Point p1_, p2_, p3_;
//    };
//
//    class Star final : public svg::Drawable {
//    public:
//        Star(svg::Point center, double outer_rad, double inner_rad, int num_rays)
//            :center_(center),
//            outer_rad_(outer_rad),
//            inner_rad_(inner_rad),
//            num_rays_(num_rays) {
//        }
//        void Draw(svg::ObjectContainer& container) const override {
//            container.Add(CreateStar(center_, outer_rad_, inner_rad_, num_rays_).SetFillColor("red"s).SetStrokeColor("black"s));
//        }
//    private:
//        svg::Point center_;
//        double outer_rad_, inner_rad_;
//        int num_rays_;
//    };
//
//    class Snowman final : public svg::Drawable {
//    public:
//        Snowman(svg::Point head_center, double head_rad)
//            : head_center_(head_center),
//            head_rad_(head_rad) {
//        }
//        void Draw(svg::ObjectContainer& container) const override {
//            container.Add(svg::Circle().SetCenter({ head_center_.x,head_center_.y + head_rad_ * 5 }).SetRadius(head_rad_ * 2).SetFillColor("rgb(240,240,240)"s).SetStrokeColor("black"s));
//            container.Add(svg::Circle().SetCenter({ head_center_.x,head_center_.y + head_rad_ * 2 }).SetRadius(head_rad_ * 1.5).SetFillColor("rgb(240,240,240)"s).SetStrokeColor("black"s));
//            container.Add(svg::Circle().SetCenter(head_center_).SetRadius(head_rad_).SetFillColor("rgb(240,240,240)"s).SetStrokeColor("black"s));
//        }
//
//    private:
//        svg::Point head_center_;
//        double head_rad_;
//    };
//
//
//}
//
//
//// Выполняет линейную интерполяцию значения от from до to в зависимости от параметра t
//uint8_t Lerp(uint8_t from, uint8_t to, double t) {
//    return static_cast<uint8_t>(std::round((to - from) * t + from));
//}
//
//// Выполняет линейную интерполяцию Rgb цвета от from до to в зависимости от параметра t
//svg::Rgb Lerp(svg::Rgb from, svg::Rgb to, double t) {
//    return { Lerp(from.red, to.red, t), Lerp(from.green, to.green, t), Lerp(from.blue, to.blue, t) };
//}
//
//int main() {
//    using namespace svg;
//    using namespace std;
//
//    ofstream out("result.svg"s);
//
//    Rgb start_color{ 0, 255, 30 };
//    Rgb end_color{ 20, 20, 150 };
//    Rgba color{ 20,40,50,0.5 };
//
//    const int num_circles = 10;
//    Document doc;
//    for (int i = 0; i < num_circles; ++i) {
//        const double t = double(i) / (num_circles - 1);
//
//        const Rgb fill_color = Lerp(start_color, end_color, t);
//
//        doc.Add(Circle()
//            .SetFillColor(fill_color)
//            .SetStrokeColor("black"s)
//            .SetCenter({ i * 20.0 + 40, 40.0 })
//            .SetRadius(15));
//    }
//
//    cout << endl;
//    {
//        Circle().SetFillColor(Rgb(1, 2, 3)).Render(cout);
//    } {
//        Circle().SetFillColor(Rgba(1, 2, 3, 0.5)).Render(cout);
//    } {
//        Circle().SetFillColor("black").Render(cout);
//    } {
//        Circle().SetFillColor("be").Render(cout);
//    } {
//        Circle().SetFillColor(NoneColor).Render(cout);
//    }
//    cout << endl;
//    

//    doc.Render(out);
//}
