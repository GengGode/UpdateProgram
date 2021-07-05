#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_UpdateProgram.h"

class UpdateProgram : public QMainWindow
{
    Q_OBJECT

public:
    UpdateProgram(QWidget *parent = Q_NULLPTR);

private:
    Ui::UpdateProgramClass ui;
};
