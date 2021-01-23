#include "progressbar.hh"

void showProgress(uint percent) {
  std::cerr << "[";
  for (uint i=0; i<50; i++) {
    if (percent/2 > i)
      std::cerr << "=";
    else
      std::cerr << " ";
  }
  std::cerr << "] " << percent <<"%" << std::endl;
}

void updateProgress(uint percent) {
  std::cerr << "\033[1A\033[K";
  showProgress(percent);
}
