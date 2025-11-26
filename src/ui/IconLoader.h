#pragma once
#include <QIcon>
#include <QPixmap>
#include <QSvgRenderer>
#include <QPainter>
#include <QApplication>
#include <QSettings>
#include <QStyleHints>
#include <QColor>

/// IconLoader: Centralized adaptive SVG icon handler for AegisCAD
/// - Loads from :/icons/*.svg
/// - Adapts to light/dark themes
/// - Generates proper Active / Disabled states automatically
/// - HiDPI-safe and lightweight (no caching issues)
class IconLoader
{
public:
    /// Loads an icon by name (e.g. "toolbar_extrude")
    /// size = nominal pixel size (default: 24)
    static QIcon load(const QString& name, int size = 24)
    {
        QString path = QString(":/icons/%1.svg").arg(name);
        return generateIconStates(path, size);
    }

private:
    static QIcon generateIconStates(const QString& path, int size)
    {
        QIcon icon;
        icon.addPixmap(tintSvg(path, size, baseColor()), QIcon::Normal, QIcon::Off);
        icon.addPixmap(tintSvg(path, size, accentColor()), QIcon::Active, QIcon::On);
        icon.addPixmap(tintSvg(path, size, disabledColor()), QIcon::Disabled, QIcon::Off);
        return icon;
    }

    /// Renders a tinted SVG at a given color
    static QPixmap tintSvg(const QString& path, int size, const QColor& tint)
    {
        QSvgRenderer renderer(path);
        QPixmap pix(size, size);
        pix.fill(Qt::transparent);

        QPainter p(&pix);
        p.setRenderHint(QPainter::Antialiasing);
        renderer.render(&p);

        // Tint overlay
        QImage img = pix.toImage();
        for (int y = 0; y < img.height(); ++y) {
            QRgb* line = reinterpret_cast<QRgb*>(img.scanLine(y));
            for (int x = 0; x < img.width(); ++x) {
                QColor c = QColor::fromRgba(line[x]);
                if (c.alpha() > 0)
                    c = blend(c, tint);
                line[x] = c.rgba();
            }
        }

        return QPixmap::fromImage(img);
    }

    /// Color blending (preserve brightness)
    static QColor blend(const QColor& base, const QColor& tint)
    {
        QColor out = base;
        out.setRedF((base.redF() * 0.5f) + (tint.redF() * 0.5f));
        out.setGreenF((base.greenF() * 0.5f) + (tint.greenF() * 0.5f));
        out.setBlueF((base.blueF() * 0.5f) + (tint.blueF() * 0.5f));
        return out;
    }

    // --------------------------
    // Color scheme helpers
    // --------------------------
    static QColor baseColor()
    {
        return isDarkMode() ? QColor("#cccccc") : QColor("#444444");
    }

    static QColor accentColor()
    {
        return QColor("#00aaff");
    }

    static QColor disabledColor()
    {
        return isDarkMode() ? QColor("#666666") : QColor("#bbbbbb");
    }

    // --------------------------
    // Theme detection
    // --------------------------
    static bool isDarkMode()
    {
#if defined(Q_OS_WIN)
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                           QSettings::NativeFormat);
        return settings.value("AppsUseLightTheme", 1).toInt() == 0;
#elif defined(Q_OS_MAC)
        return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#else
        return QGuiApplication::palette().color(QPalette::Window).lightness() < 128;
#endif
    }
};
