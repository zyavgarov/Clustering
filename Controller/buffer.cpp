#include "../Controller.h"

int Controller::buffer_add_cloud (int id) {
    // adds cloud with id to buffer
    // returns -1 if field is readonly
    if (Field::readonly ()) {
        return -1;
    }
    Field::buf.add (Field::cloud ()[id]);
    return 0;
}

int Controller::buffer_unload ()  {
    // unloads buffer to field
    // returns -1 if field is readonly
    if (Field::readonly ()) {
        return -1;
    }
    Field::add (Field::buf.unload ());
    return 0;
}


int Controller::buffer_shift (double x, double y)  {
    // shifts cloud in buffer to vector (x, y)
    if (Field::readonly ()) {
        return -1;
    }
    Field::buf.shift (x, y);
    return 0;
}

int Controller::buffer_zoom (double k)  {
    //zooms cloud in buffer to coefficient k
    if (Field::readonly ()) {
        return -1;
    }
    Field::buf.zoom (k);
    return 0;
}

int Controller::buffer_mirror () {
    //zooms cloud in buffer to coefficient k
    if (Field::readonly ()) {
        return -1;
    }
    Field::buf.mirror ();
    return 0;
}

int Controller::buffer_rotate (double angle) {
    // rotates cloud in buffer to angle
    if (Field::readonly ()) {
        return -1;
    }
    Field::buf.rotate (angle);
    return 0;
}

int Controller::buffer_erase () {
    //clears the buffer
    if (Field::readonly ()) {
        return -1;
    }
    Field::buf.erase();
    return 0;
}
