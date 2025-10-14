#include <QApplication>
#include <QGuiApplication>
#include <QStyleHints>

#include "Window.hpp"
#include "Theme.hpp"

static void applyPaletteForScheme(Qt::ColorScheme scheme) {
    // Use Fusion so our palette is respected across platforms.
    // QApplication::setStyle("Fusion");
    if (scheme == Qt::ColorScheme::Dark) {
        qApp->setPalette(makeDarkPalette());
    } else {
        qApp->setPalette(makeLightPalette());
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    applyPaletteForScheme(QGuiApplication::styleHints()->colorScheme());

    // Live updates when the system theme changes (Qt 6.6+)
    QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
                     &app, [](Qt::ColorScheme scheme){
                         applyPaletteForScheme(scheme);
                     });

    Window mainWindow;
    mainWindow.resize(1024, 800);
    mainWindow.show();

    return app.exec();
}