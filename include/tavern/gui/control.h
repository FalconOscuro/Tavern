#ifndef CONTROL_H
#define CONTROL_H

namespace tavern::gui {

// base class for all gui components
class control
{
public:
    
    virtual ~control() = default;

    virtual void draw() const = 0;
}; /* end of class control */

} /* namespace tavern::gui */

#endif /* end of include guard: CONTROL_H */
