#include "progressbar.hh"

void showProgress(unsigned percent) {
  std::cerr << "[";
  for (unsigned i=0; i<50; i++) {
    if (percent/2 > i)
      std::cerr << "=";
    else
      std::cerr << " ";
  }
  std::cerr << "] " << percent <<"%" << std::endl;
}

void updateProgress(unsigned percent) {
  std::cerr << "\033[1A\033[K";
  showProgress(percent);
}
