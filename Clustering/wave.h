#ifndef INTERFACE4__WAVE_H_
#define INTERFACE4__WAVE_H_
#include "../Field.h"

class wave {
  int search_id;
  int err_{};
  wave (wave const &);
 public:
  explicit wave (int search_id);
  int err () const;
};

#endif //INTERFACE4__WAVE_H_
