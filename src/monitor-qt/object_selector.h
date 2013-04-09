#ifndef OBJECT_SELECTOR_H
#define OBJECT_SELECTOR_H

#include <QWidget>

namespace Ui {
class object_selector;
}

class object_selector : public QWidget
{
    Q_OBJECT
    
public:
    explicit object_selector(QWidget *parent = 0);
    ~object_selector();
    
private:
    Ui::object_selector *ui;
};

#endif // OBJECT_SELECTOR_H
