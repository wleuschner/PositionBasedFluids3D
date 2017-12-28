#ifndef __ABSTRACT_FORCE_H
#define __ABSTRACT_FORCE_H
#include<glm/glm.hpp>

class AbstractForce
{
public:
    AbstractForce();
    virtual glm::vec3 execute(const glm::vec3& pos) = 0;
private:
};

#endif
