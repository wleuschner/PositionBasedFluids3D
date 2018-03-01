#include"PlacementDialog.h"

PlacementDialog::PlacementDialog(QWidget* parent) : QDialog(parent)
{
    ui.setupUi(this);
}

glm::vec3 PlacementDialog::getDisplacement()
{
    glm::vec3 t;
    t.x = ui.spinBoxX->value();
    t.y = ui.spinBoxY->value();
    t.z = ui.spinBoxZ->value();
    return t;
}

bool PlacementDialog::isSolid()
{
    return ui.checkBoxSolid->isChecked();
}
