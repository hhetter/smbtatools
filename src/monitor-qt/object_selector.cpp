#include "object_selector.h"
#include "ui_object_selector.h"

object_selector::object_selector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::object_selector)
{
    ui->setupUi(this);
}

object_selector::~object_selector()
{
    delete ui;
}
