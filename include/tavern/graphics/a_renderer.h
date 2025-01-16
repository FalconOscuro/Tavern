#ifndef A_RENDERER_H
#define A_RENDERER_H

namespace tavern::graphics {

class a_renderer
{
public:

    virtual bool init() = 0;
    virtual void update() = 0;

}; /* end of class a_renderer */

} /* end of namespace tavern::graphics */

#endif /* end of define guard A_RENDERER_H */
