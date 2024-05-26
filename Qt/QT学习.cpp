桥接网卡:!!!!!!以太网+无线网!!!!!

P1,C++
    类的实例化-对象，类的成员变量，public,protected,private,ps:默认private  从栈中实例化对象->对象初始化->系统自动回收对象; 从堆中实例化对象->地址指针+new->对象初始化
构造函数:function name = class name,会在类实例化时被调用； 析构函数:~function name = class name,会在类被销毁时被调用;
类的继承:基类-派生类，this指针为本类指针(类内有成员变量、成员函数)；函数重载:同名函数 ； 命名空间namespace A ； 
父对象:对象树，饭桌(父)和饭菜(子)=对象析构(父对象先析构子对象)，

P2,
    信号与槽:学校 通知 学生 上课 = 发送者(对象) 信号(函数) 接收者(对象) 信号(函数). 帮助文档!!!
    样式表-HTML使用；图像化+c++编程.                

0,信号与槽:signal与slot.    
1,控件
(1)普通按钮(QPushButton)、工具栏按钮(QToolButton)-带图标、工具栏(QToolBar) 单选框(QRidioButton) 复选框(QCheckBox)
命令链接按钮(QCommandLinkButton)-url   对话框(QDialogButtonBox)-qt按钮与自定义按钮  

(2)选项槽(QComboBox)-index操作  字体属性(QFontComboBox)  单行输入(QLineEdit)  文本编辑(QTextEdit)-全选/清除  微调(QSpinBox)
微调浮点(QDoubleSpinBox)   时间和日期(QDateTimeEdit、QTimeEdit、QDateEdit)  刻度盘(QDial)  滚动条(QScrollBar)  滑块(QSlider)
组合键(QKeySequenceEdit)  

(3)文字与图显示(QLabel)  日历(QCalendarWidget)-日期函数  时间显示与计时(QLCDNumber、QTimer)  加载进度(QProgressBar)  
画图与框架样式(QFrame)  

(4)文本浏览器(QTextBrowser)  xx_bar(栏+QAction)   图像浏览器(QGraphicsView->QGraphicsScene)  

(5)布局空间与部件(QHBoxLayout<->QWidget):布局的按钮部件,垂直部件的布局  widget窗口部件容纳->部件->部件布局xx_layout   多维数组布局(QGridLayou)
行列布局(QFormLayout)  布局加入间隔(QSpacerItem)

(6)组织框(QGroupBox-)  滚动框(QScrollArea)  工具箱(QToolBox->groupBox)  多页面小部件(QTabWidget)  堆栈窗口部件(QStackedWidget-1次1部件)
新建窗口(QMdiArea-QMdiSubWindow-从)   停靠窗口(QDockWidget)

(7)列表视图(QListView)-实时修改text  树状视图(QTreeView)-1/2级标题  表单(QStandardItemModel->QTableView)  级联列表(QStandardItem->QStandardItemModel->QColumnView)
添加项文件(QListWidget-音乐)  可编辑表单(QTableWidgetItem->QTableWidgetItem) 

//-----------------------------------------------------------
2,控件及文件操作
读取文件(QFile、QTextStream)  绘图(QPainter-paintEvent)  图表(QChart->QChartView)

3,多线程
主子线程QThread->WorkerThread   TCP的服务和客户端(QHostAddress、QTcpSocket)  UDP()-单播、广播、组播

4,NET
网络下载(QNetworkAccessManager-QNetworkRequest-QNetworkReply)

5,多媒体
(1)音效文件(QSound)

(2)媒体播放器(QMediaPlayer) 媒体列表(QMediaPlaylist)  按钮信号-信号槽(action/state)  

(3)录音(QAudioRecorder-QAudioProbe-)

6,数据库
数据库连接类(QSqlDatabase)  操作类型(QSqlTableModel)  查询数据(QSqlQueryModel) 数据映射(QDataWidgetMapper)  选择模型(QItemSelectionModel)
//-----------------------------------------------------------
函数覆写 

#2024.5.26


