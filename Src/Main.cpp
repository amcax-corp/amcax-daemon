/* ===================================================================
* Copyright (C) 2023 Hefei Jiushao Intelligent Technology Co., Ltd. 
* All rights reserved.
*
* This software is licensed under the GNU Affero General Public License 
* v3.0 (AGPLv3.0) or a commercial license. You may choose to use this 
* software under the terms of either license.
*
* For more information about the AGPLv3.0 license, please visit: 
* https://www.gnu.org/licenses/agpl-3.0.html
* For licensing inquiries or to obtain a commercial license, please 
* contact Hefei Jiushao Intelligent Technology Co., Ltd.
* ===================================================================
* Author: 
*/
#include "Windows/ACAMWindow.h"
#include <QtWidgets/qapplication.h>
#include <QCoreApplication>
#include <QGuiApplication>

int main(int argc, char** argv) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);
	// Read command lines arguments.
	QApplication application(argc, argv);

	// Instantiate the viewer.
	ACAMWindow window;

	window.setWindowTitle("九韶精灵");

	// Make the viewer window visible on screen.
	window.show();

	// Run main loop.
	return application.exec();

}
