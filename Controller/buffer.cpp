#include "../Controller.h"

int Controller::buffer_add_cloud (int id) const {
    // adds cloud with id to buffer
    // returns -1 if field is readonly
    if (field_->readonly ()) {
        return -1;
    }
    field_->buf.add (field_->cloud ()[id]);
    return 0;
}

int Controller::buffer_unload () const {
    // unloads buffer to field
    // returns -1 if field is readonly
    if (field_->readonly ()) {
        return -1;
    }
    field_->add (field_->buf.unload ());
    return 0;
}


int Controller::buffer_shift (double x, double y) const {
    // shifts cloud in buffer to vector (x, y)
    if (field_->readonly ()) {
        return -1;
    }
    field_->buf.shift (x, y);
    return 0;
}

int Controller::buffer_zoom (double k) const {
    //zooms cloud in buffer to coefficient k
    if (field_->readonly ()) {
        return -1;
    }
    field_->buf.zoom (k);
    return 0;
}

int Controller::buffer_mirror () const {
    //zooms cloud in buffer to coefficient k
    if (field_->readonly ()) {
        return -1;
    }
    field_->buf.mirror ();
    return 0;
}

int Controller::buffer_rotate (double angle) const {
    // rotates cloud in buffer to angle
    if (field_->readonly ()) {
        return -1;
    }
    field_->buf.rotate (angle);
    return 0;
}

int Controller::buffer_erase () const {
    //clears the buffer
    if (field_->readonly ()) {
        return -1;
    }
    field_->buf.erase();
    return 0;
}
