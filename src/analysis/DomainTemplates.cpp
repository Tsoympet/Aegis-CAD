#include "DomainTemplates.h"

QVector<DomainTemplate> DomainTemplates::defaults()
{
    return {
        { "Car Chassis", "Static frame with suspension load points", 7850, 2.1e11, 0.3 },
        { "Ship Deck", "Marine deck structure with distributed load", 7800, 2.0e11, 0.28 },
        { "Aircraft Wing", "Aluminum wing panel under bending", 2700, 7.0e10, 0.33 },
        { "Armor Hull", "Composite steel armor section", 7850, 2.05e11, 0.29 },
        { "Generic Steel", "Default isotropic steel template", 7850, 2.1e11, 0.3 }
    };
}
