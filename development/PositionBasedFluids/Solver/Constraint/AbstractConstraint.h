#ifndef __ABSTRACT_CONSTRAINT_H
#define __ABSTRACT_CONSTRAINT_H

class AbstractConstraint
{
public:
    AbstractConstraint();
    virtual void execute() = 0;
private:
};

#endif
