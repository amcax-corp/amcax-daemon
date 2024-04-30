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
#include <QtWidgets/qapplication.h>
#include <QCoreApplication>
#include <QGuiApplication>
#include "Windows/RibbonViewer.h"
#include "SARibbonBar.h"
#include <qtextcodec.h>
#include <QSharedMemory>

int main(int argc, char** argv) {

	// 仅仅允许一个进程
	QSharedMemory singleton("AmcaxDaemon-SINGLETON");
	if (!singleton.create(1)) {
		return -1;
	}


	SARibbonBar::initHighDpi();


	QApplication application(argc, argv);
	QFont f = application.font();
	f.setFamily(u8"微软雅黑");
	application.setFont(f);

	// Instantiate the viewer.
	RibbonViewer window;
	window.setRibbonTheme(SARibbonMainWindow::RibbonThemeOffice2016Blue);

	//window.setWindowTitle("九韶精灵");
	//ndow.initFile();
	// Make the viewer window visible on screen.
	//window.showFullScreen();

	// Run main loop.
	return  (application.exec());

}
