#ifndef MESH_H
#define MESH_H

namespace tavern::graphics::generic {

class mesh
{
public:

    virtual ~mesh(){};

    virtual void draw() const = 0;

}; /* end of class mesh */

} /* end of namespace tavern::graphics::generic */

#endif /* end of define guard MESH_H */
