#ifndef __PLACEMENT_DIALOG_
#define __PLACEMENT_DIALOG_
#include<QDialog>
#include<glm/glm.hpp>
#include"ui_PlacementDialog.h"

class PlacementDialog : public QDialog
{
    Q_OBJECT
public:
    PlacementDialog(QWidget* parent=0);
    glm::vec3 getDisplacement();
    bool isSolid();
private:
    Ui::PlacementDialog ui;
};

#endif
