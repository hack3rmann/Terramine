#pragma once
#ifndef GUI_OBJECT_H_
#    define GUI_OBJECT_H_

class GUIObject {
protected:
    float posX, posY, width, height;
    GUIObject(float posX, float posY, float width, float height);
    GUIObject();
};

#endif  // !GUI_OBJECT_H_
