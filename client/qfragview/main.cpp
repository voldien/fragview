#include<FragCore.h>
#include<QApplication>
#include<QSplashScreen>

int main(int argc, const char **argv) {

	QApplication app(argc, argv);

	QPixmap pixmap("splash.jpg");
	QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);

	return EXIT_SUCCESS;
}
