#pragma once

namespace geo {

struct Coordinates {
    double lat; // Широта
    double lng; // Долгота

    bool operator==(const Coordinates& other) const {
        return (((lat - other.lat) < 10e-5) && ((lng - other.lng) < 10e-5));
    }
};

double ComputeDistance(Coordinates from, Coordinates to);

}  // namespace geo