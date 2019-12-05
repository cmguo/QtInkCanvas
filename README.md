# QtInkCanvas
A porting of WPF InkCanvas to Qt enviriment.

# 支持下列功能特性：
+ 笔迹书写（平滑、压力感应）
+ 选择（点选、套索）
+ 擦除（笔迹擦除、像素擦除）
+ 复制粘贴
+ 辅组视觉（选择框、套索，光标）
+ 手势识别

# 方案
+ C# 转 C++
+ QEvent - > EventArgs
+ QWidget/QGraphicsItem -> Visual/UIElement
+ QPainterPath -> Geometry
+ Geometry+QBrush+QPen -> Drawing
+ Signal/Slot -> EventHandler
+ 简单实现 DependencyObject DependencyProperty 和 RouteEvent
